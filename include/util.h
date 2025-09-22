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

struct DDSHeader
{
    uint32_t dwMagic;
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[11];

    struct
    {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwABitMask;
    } ddpf;

    struct
    {
        uint32_t dwCaps1;
        uint32_t dwCaps2;
        uint32_t dwDDSX;
        uint32_t dwReserved;
    } caps;

    uint32_t dwReserved2;
};

// Decode next UTF-8 codepoint starting at 'it'. Advances 'it'.
// Returns 0xFFFD (replacement char) on error.
uint32_t utf8Next(const char*& it, const char* end);

// Encode UTF-8 codepoint
void utf8Encode(uint32_t cp, std::string& out);

// Truncate UTF-8 string with "..." if wider than maxWidth
std::string fitTextWithEllipsisUtf8(const std::string& text, float maxWidth);
bool convertMeshToGlb(const fs::path& inputPath, const fs::path& outputPath);
bool convertImageToDxt5(const fs::path& inputPath, const fs::path& outputPath);

#endif // header guard

