#pragma once

#include <vector>
#include <cmath>


class ImageCompression {
public:
    static std::vector<double> performDCT(const std::vector<double>& block);
    static std::vector<double> performIDCT(const std::vector<double>& block);
    static std::vector<std::vector<double>> compressImage(const std::vector<std::vector<double>>& image, int blockSize);
    static std::vector<std::vector<double>> decompressImage(const std::vector<std::vector<double>>& compressedImage, int blockSize);
};