#ifndef IMAGE_COMPRESSOR_H
#define IMAGE_COMPRESSOR_H

#include <vector>
#include <unordered_map>
#include <cstdint>

struct ImageHeader {
    uint16_t fileType;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offsetToPixelArray;
    uint32_t sizeOfHeader;
    uint32_t width;
    uint32_t height;
    uint16_t colorPlanes;
    uint16_t bitsPerPixel;
    uint32_t compressionMethod;
    uint32_t sizeOfPixelArray;
    uint32_t horizontalResolution;
    uint32_t verticalResolution;
    uint32_t colorTableEntries;
    uint32_t importantColors;
};

struct HuffmanNode {
    uint8_t value;
    int frequency;
    HuffmanNode *left, *right;

    HuffmanNode(uint8_t val, int freq, HuffmanNode *l = nullptr, HuffmanNode *r = nullptr);
    bool operator>(const HuffmanNode& other) const;
};

void buildHuffmanCode(HuffmanNode* root, std::unordered_map<uint8_t, std::string>& codes, std::string code = "");

std::vector<uint8_t> compressHuffman(const std::vector<uint8_t>& pixelData);
std::vector<uint8_t> decompressHuffman(const std::vector<uint8_t>& compressedData, const std::unordered_map<uint8_t, std::string>& huffmanCodes);

#endif  // IMAGE_COMPRESSOR_H