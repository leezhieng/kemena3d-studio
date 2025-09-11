#ifndef UTIL_H
#define UTIL_H

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <future>
#include <atomic>
#include <mutex>

#include <filesystem>

namespace fs = std::filesystem;

// Decode next UTF-8 codepoint starting at 'it'. Advances 'it'.
// Returns 0xFFFD (replacement char) on error.
inline uint32_t utf8Next(const char*& it, const char* end)
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
inline void utf8Encode(uint32_t cp, std::string& out)
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

// Truncate UTF-8 string with "..." if wider than maxWidth
inline std::string fitTextWithEllipsisUtf8(const std::string& text, float maxWidth)
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

inline bool convertMeshToGlb(const fs::path& inputPath, const fs::path& outputPath)
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

#endif // header guard

