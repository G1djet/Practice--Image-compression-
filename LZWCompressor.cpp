#include "LZWCompressor.h"

std::vector<uint8_t> LZWCompressor::compressLZW(const std::vector<uint8_t>& imageData) {
    std::unordered_map<std::string, int> codebook = buildInitialCodebook(imageData);
    std::vector<int> compressedData = encodeImageData(imageData, codebook);

    std::vector<uint8_t> compressedBytes;
    for (int code : compressedData) {
        int bytesPerCode = (code < 256) ? 1 : 2;
        for (int i = 0; i < bytesPerCode; i++) {
            compressedBytes.push_back(static_cast<uint8_t>(code >> (8 * (bytesPerCode - 1 - i))));
        }
    }

    return compressedBytes;
}

std::vector<uint8_t> LZWCompressor::decompressLZW(const std::vector<uint8_t>& compressedData) {
    std::unordered_map<int, std::string> codebook = buildInverseCodebook(buildInitialCodebook({}));
    std::vector<int> compressedCodes;

    for (size_t i = 0; i < compressedData.size(); i += 2) {
        int code = (compressedData[i] << 8) | compressedData[i + 1];
        compressedCodes.push_back(code);
    }

    return decodeCompressedData(compressedCodes, codebook);
}

std::unordered_map<std::string, int> LZWCompressor::buildInitialCodebook(const std::vector<uint8_t>& imageData) {
    std::unordered_map<std::string, int> codebook;
    for (uint8_t i = 0; i < 256; i++) {
        codebook[std::string(1, static_cast<char>(i))] = i;
    }
    return codebook;
}

std::unordered_map<int, std::string> LZWCompressor::buildInverseCodebook(const std::unordered_map<std::string, int>& codebook) {
    std::unordered_map<int, std::string> inverseCodebook;
    for (const auto& [key, value] : codebook) {
        inverseCodebook[value] = key;
    }
    return inverseCodebook;
}

std::vector<int> LZWCompressor::encodeImageData(const std::vector<uint8_t>& imageData, const std::unordered_map<std::string, int>& codebook) {
    std::vector<int> compressedData;
    std::string currentString = "";
    std::unordered_map<std::string, int> localCodebook = codebook;
    int nextCode = localCodebook.size();

    for (uint8_t byte : imageData) {
        std::string newString = currentString + static_cast<char>(byte);
        auto it = localCodebook.find(newString);
        if (it != localCodebook.end()) {
            currentString = newString;
        } else {
            compressedData.push_back(localCodebook.at(currentString));
            localCodebook.emplace(newString, nextCode++);
            currentString = std::string(1, static_cast<char>(byte));
        }
    }
    if (!currentString.empty()) {
        compressedData.push_back(localCodebook.at(currentString));
    }
    return compressedData;
}

std::vector<uint8_t> LZWCompressor::decodeCompressedData(const std::vector<int>& compressedData, const std::unordered_map<int, std::string>& codebook) {
    std::vector<uint8_t> decompressedData;
    std::string previousString = "";
    std::unordered_map<int, std::string> localCodebook = codebook;
    int nextCode = localCodebook.size();

    for (int code : compressedData) {
        std::string currentString;
        if (localCodebook.find(code) != localCodebook.end()) {
            currentString = localCodebook.at(code);
        } else {
            currentString = previousString + previousString[0];
        }
        decompressedData.insert(decompressedData.end(), currentString.begin(), currentString.end());
        if (!previousString.empty()) {
            localCodebook.emplace(nextCode++, previousString + currentString[0]);
        }
        previousString = currentString;
    }
    return decompressedData;
}