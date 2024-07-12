#include "RLECompressor.h"
#include <vector>

std::vector<uint8_t> compressRLE(const std::vector<uint8_t>& pixelData, uint16_t bitsPerPixel) {
    std::vector<uint8_t> compressedData;
    int count = 1;
    uint8_t currentPixel = pixelData[0];

    for (size_t i = 1; i < pixelData.size(); i++) {
        if (pixelData[i] == currentPixel && count < 255) {
            count++;
        } else {
            if (count > 2 || (currentPixel == 0 && count == 2)) {
                compressedData.push_back(static_cast<uint8_t>(count));
                compressedData.push_back(currentPixel);
            } else {
                for (int j = 0; j < count; j++) {
                    compressedData.push_back(currentPixel);
                }
            }
            currentPixel = pixelData[i];
            count = 1;
        }
    }

    if (count > 2 || (currentPixel == 0 && count == 2)) {
        compressedData.push_back(static_cast<uint8_t>(count));
        compressedData.push_back(currentPixel);
    } else {
        for (int j = 0; j < count; j++) {
            compressedData.push_back(currentPixel);
        }
    }

    if (compressedData.size() % 2 != 0) {
        compressedData.push_back(0);
    }

    return compressedData;
}

std::vector<uint8_t> decompressRLE(const std::vector<uint8_t>& compressedData, uint32_t sizeOfPixelArray) {
std::vector<uint8_t> decompressedData;
    size_t i = 0;
    while (i < compressedData.size() && decompressedData.size() < sizeOfPixelArray) {
        uint8_t count = compressedData[i++];
        uint8_t pixel = compressedData[i++];
        if (count == 0) {
            if (compressedData[i] == 0) {
                i++;
            } 
        else {
                count = compressedData[i++];
                for (uint8_t j = 0; j < count && decompressedData.size() < sizeOfPixelArray; j++) {
                    decompressedData.push_back(compressedData[i++]);
                }
                if (count % 2 != 0) {
                    i++;  
                }
            }
        } 
else {
             for (uint8_t j = 0; j < count && decompressedData.size() < sizeOfPixelArray; j++) {
                decompressedData.push_back(pixel);
            }
        }
    }
    return decompressedData;
}