#ifndef IMAGE_COMPRESSOR_H
#define IMAGE_COMPRESSOR_H

#include <vector>
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

std::vector<uint8_t> compressRLE(const std::vector<uint8_t>& pixelData, uint16_t bitsPerPixel);
std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& compressedData, uint32_t sizeOfPixelArray);

#endif  // IMAGE_COMPRESSOR_H