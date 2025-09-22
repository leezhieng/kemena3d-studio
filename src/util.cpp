#include "util.h"

#include <stb/stb_image.h>
#define STB_DXT_IMPLEMENTATION
#include <stb/stb_dxt.h>

#include <fstream>  // for std::ofstream
#include <vector>
#include <iostream>
#include <filesystem>
#include <cstring>   // for memcpy

#include <imgui.h>

uint32_t utf8Next(const char*& it, const char* end)
{
	if (it >= end) return 0;

	unsigned char c = static_cast<unsigned char>(*it++);
	if (c < 0x80) return c; // ASCII

	// Multibyte
	int extra = 0;
	uint32_t cp = 0;
	if ((c & 0xE0) == 0xC0)
	{
		cp = c & 0x1F;
		extra = 1;
	}
	else if ((c & 0xF0) == 0xE0)
	{
		cp = c & 0x0F;
		extra = 2;
	}
	else if ((c & 0xF8) == 0xF0)
	{
		cp = c & 0x07;
		extra = 3;
	}
	else return 0xFFFD;

	for (int i = 0; i < extra; i++)
	{
		if (it >= end) return 0xFFFD;
		unsigned char cc = static_cast<unsigned char>(*it);
		if ((cc & 0xC0) != 0x80) return 0xFFFD;
		cp = (cp << 6) | (cc & 0x3F);
		++it;
	}
	return cp;
}

// Encode UTF-8 codepoint
void utf8Encode(uint32_t cp, std::string& out)
{
	if (cp < 0x80) out.push_back((char)cp);
	else if (cp < 0x800)
	{
		out.push_back((char)(0xC0 | (cp >> 6)));
		out.push_back((char)(0x80 | (cp & 0x3F)));
	}
	else if (cp < 0x10000)
	{
		out.push_back((char)(0xE0 | (cp >> 12)));
		out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
		out.push_back((char)(0x80 | (cp & 0x3F)));
	}
	else
	{
		out.push_back((char)(0xF0 | (cp >> 18)));
		out.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
		out.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
		out.push_back((char)(0x80 | (cp & 0x3F)));
	}
}

std::string fitTextWithEllipsisUtf8(const std::string& text, float maxWidth)
{
	if (text.empty()) return "";

	if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
		return text;

	const std::string ell = "...";

	const char* begin = text.c_str();
	const char* end   = text.c_str() + text.size();
	const char* it    = begin;

	std::string out;
	while (it < end)
	{
		const char* prev = it;
		uint32_t cp = utf8Next(it, end);

		std::string tmp = out;
		utf8Encode(cp, tmp);
		tmp += ell;

		float w = ImGui::CalcTextSize(tmp.c_str()).x;
		if (w > maxWidth) break;

		out.append(prev, it); // append original UTF-8 slice
	}

	out += ell;
	return out;
}

bool convertMeshToGlb(const fs::path& inputPath, const fs::path& outputPath)
{
	Assimp::Importer importer;

	// Ensure we load all necessary data, including animations
	const aiScene* scene = importer.ReadFile(
							   inputPath.string(),
							   aiProcess_Triangulate            |
							   aiProcess_GenNormals             |
							   aiProcess_JoinIdenticalVertices  |
							   aiProcess_LimitBoneWeights       |
							   aiProcess_SortByPType            |
							   aiProcess_ImproveCacheLocality   |
							   aiProcess_ValidateDataStructure
						   );

	if (!scene)
	{
		std::cerr << "Assimp failed to load " << inputPath
				  << ": " << importer.GetErrorString() << "\n";
		return false;
	}

	// Check if scene has animations
	if (scene->mNumAnimations > 0)
	{
		std::cout << inputPath << " contains " << scene->mNumAnimations
				  << " animation(s). They will be exported.\n";
	}

	Assimp::Exporter exporter;
	aiReturn ret = exporter.Export(scene, "glb2", outputPath.string());

	if (ret != AI_SUCCESS)
	{
		std::cerr << "Assimp failed to export " << outputPath
				  << ": " << exporter.GetErrorString() << "\n";
		return false;
	}

	std::cout << "Converted: " << inputPath << " -> " << outputPath << "\n";
	return true;
}

bool convertImageToDxt5(const fs::path& inputPath, const fs::path& outputPath)
{
    int w, h, comp;
    unsigned char* rgba = stbi_load(inputPath.string().c_str(), &w, &h, &comp, 4);
    if (!rgba)
    {
        std::cerr << "Failed to load image: " << inputPath << "\n";
        return false;
    }

    int blockCountX = (w + 3) / 4;
    int blockCountY = (h + 3) / 4;
    int blockSize = 16; // DXT5
    size_t bufferSize = blockCountX * blockCountY * blockSize;

    std::vector<unsigned char> compressed(bufferSize);

    unsigned char* outPtr = compressed.data();
    for (int by = 0; by < h; by += 4)
    {
        for (int bx = 0; bx < w; bx += 4)
        {
            unsigned char block[64] = {0}; // 4x4 RGBA pixels
            for (int yy = 0; yy < 4; yy++)
            {
                for (int xx = 0; xx < 4; xx++)
                {
                    int sx = bx + xx;
                    int sy = by + yy;
                    unsigned char* dst = &block[4 * (yy * 4 + xx)];
                    if (sx < w && sy < h)
                    {
                        unsigned char* src = rgba + 4 * (sy * w + sx);
                        memcpy(dst, src, 4);
                    }
                    else
                    {
                        memset(dst, 0, 4);
                    }
                }
            }
            stb_compress_dxt_block(outPtr, block, 1, STB_DXT_HIGHQUAL);
            outPtr += blockSize;
        }
    }

    stbi_image_free(rgba);

    // Build DDS header
    DDSHeader header{};
    header.dwMagic = 0x20534444; // "DDS "
    header.dwSize = 124;
    header.dwFlags = 0x0002100F; // caps | height | width | pixelformat | linearsize
    header.dwHeight = h;
    header.dwWidth = w;
    header.dwPitchOrLinearSize = static_cast<uint32_t>(bufferSize);
    header.dwMipMapCount = 0;

    header.ddpf.dwSize = 32;
    header.ddpf.dwFlags = 0x00000004; // DDPF_FOURCC
    header.ddpf.dwFourCC = ('D') | ('X' << 8) | ('T' << 16) | ('5' << 24);

    header.caps.dwCaps1 = 0x1000; // texture

    // Write DDS file
    std::ofstream ofs(outputPath, std::ios::binary);
    if (!ofs)
    {
        std::cerr << "Failed to open output file: " << outputPath << "\n";
        return false;
    }

    ofs.write(reinterpret_cast<const char*>(&header), sizeof(header));
    ofs.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
    ofs.close();

    std::cout << "Converted: " << inputPath << " -> " << outputPath << "\n";
    return true;
}
