#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <memory>

class ImageCompressor {
public:
    virtual std::vector<unsigned char> compress(const std::vector<unsigned char>& image) = 0;
    virtual std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressedData) = 0;
};

class RLECompressor : public ImageCompressor {
public:
    std::vector<unsigned char> compress(const std::vector<unsigned char>& image) override {
        return {};
    }

    std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressedData) override {
        return {};
    }
};

class DCTCompressor : public ImageCompressor {
public:
    std::vector<unsigned char> compress(const std::vector<unsigned char>& image) override {
        return {};
    }

    std::vector<unsigned char> decompress(const std::vector<unsigned char>& compressedData) override {
        return {};
    }
};

class ImageViewer {
private:
    std::vector<unsigned char> originalImage;
    std::vector<unsigned char> compressedImage;
    std::unique_ptr<ImageCompressor> compressor;

public:
    void loadImage(const std::string& filename) {
    }

    void compress(ImageCompressor& compressor) {
        compressedImage = compressor.compress(originalImage);
    }

    void decompress() {
        originalImage = compressor->decompress(compressedImage);
    }

    void setCompressor(std::unique_ptr<ImageCompressor> newCompressor) {
        compressor = std::move(newCompressor);
    }
};
