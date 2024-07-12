#ifndef LZW_COMPRESSOR_H
#define LZW_COMPRESSOR_H

#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

class LZWCompressor {
public:
    static std::vector<uint8_t> compressLZW(const std::vector<uint8_t>& imageData);
    static std::vector<uint8_t> decompressLZW(const std::vector<uint8_t>& compressedData);

private:
    static std::unordered_map<std::string, int> buildInitialCodebook(const std::vector<uint8_t>& imageData);
    static std::unordered_map<int, std::string> buildInverseCodebook(const std::unordered_map<std::string, int>& codebook);
    static std::vector<int> encodeImageData(const std::vector<uint8_t>& imageData, const std::unordered_map<std::string, int>& codebook);
    static std::vector<uint8_t> decodeCompressedData(const std::vector<int>& compressedData, const std::unordered_map<int, std::string>& codebook);
};

#endif // LZW_COMPRESSOR_H