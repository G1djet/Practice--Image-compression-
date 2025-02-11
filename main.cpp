#include "LZWCompressor.cpp"
#include "Huffman.cpp"
#include "RLECompressor.cpp"
#include "DCTCompression.cpp"
///#include "FractalCompression.cpp"

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

int main() {
    std::string inputFilePath;
    std::cout << "Enter the path to the BMP or TIFF image file: ";
    std::getline(std::cin, inputFilePath);

    if (!fs::exists(inputFilePath)) {
        std::cerr << "Error: File not found." << std::endl;
        return 1;
    }

    std::vector<uint8_t> originalData;
    std::ifstream file(inputFilePath, std::ios::binary);
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        originalData.resize(fileSize);
        file.read(reinterpret_cast<char*>(originalData.data()), fileSize);
        file.close();
    } else {
        std::cerr << "Error: Failed to open file." << std::endl;
        return 1;
    }

    int compressionAlgorithm;
    std::cout << "Select compression algorithm:" << std::endl;
    std::cout << "1. LZW" << std::endl;
    std::cout << "2. Huffman" << std::endl;
    std::cout << "3. RLE" << std::endl;
    std::cout << "4. DCT" << std::endl;
    ///std::cout << "5. Fractal" << std::endl;
    std::cout << "Enter your choice (1-5): ";
    std::cin >> compressionAlgorithm;

    std::vector<uint8_t> compressedData;
    double compressionRatio = 0.0;
    switch (compressionAlgorithm) {
        case 1: { // LZW
            LZWCompressor lzwCompressor;
            std::vector<uint8_t> compressedData = lzwCompressor.compressLZW(originalData);
            compressionRatio = static_cast<double>(compressedData.size()) / originalData.size();

            std::vector<uint8_t> decompressedData = lzwCompressor.decompressLZW(compressedData);

            bool isDecompressionCorrect = (originalData == decompressedData);
            if (isDecompressionCorrect) {
                std::cout << "Decompression successful." << std::endl;
            } else {
                std::cerr << "Decompression failed." << std::endl;
            }
            break;
        }
        case 2: { // Huffman
            std::vector<uint8_t> compressedData = compressHuffman(originalData);
            compressionRatio = static_cast<double>(compressedData.size()) / originalData.size();

            std::unordered_map<uint8_t, std::string> huffmanCodes;
            HuffmanNode* root = buildHuffmanTree(originalData);
            buildHuffmanCode(root, huffmanCodes);
            std::vector<uint8_t> decompressedData = decompressHuffman(compressedData, huffmanCodes);

            bool isDecompressionCorrect = (originalData == decompressedData);
            if (isDecompressionCorrect) {
                std::cout << "Decompression successful." << std::endl;
            } else {
                std::cerr << "Decompression failed." << std::endl;
            }
            break;
        }
        case 3: { // RLE
            std::vector<uint8_t> compressedData = compressRLE(originalData, 8); // Предполагаем, что изображение имеет 8 бит на пиксель
            compressionRatio = static_cast<double>(compressedData.size()) / originalData.size();

            std::vector<uint8_t> decompressedData = decompressRLE(compressedData, originalData.size());

            bool isDecompressionCorrect = (originalData == decompressedData);
            if (isDecompressionCorrect) {
                std::cout << "Decompression successful." << std::endl;
            } else {
                std::cerr << "Decompression failed." << std::endl;
            }
            break;
        }
        case 4: { // DCT
            int blockSize = 8;
            ImageCompression dctCompressor;

            int width = static_cast<int>(std::sqrt(originalData.size()));
            int height = width;
            std::vector<std::vector<double>> originalImageData(height, std::vector<double>(width));
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    originalImageData[i][j] = static_cast<double>(originalData[i * width + j]);
                }
            }

            std::vector<std::vector<double>> compressedImage = dctCompressor.compressImage(originalImageData, blockSize);

            int totalElements = height * width;
            int compressedElements = static_cast<int>(compressedImage.size() * compressedImage[0].size());
            compressionRatio = static_cast<double>(compressedElements) / totalElements;

            std::vector<std::vector<double>> decompressedImage = dctCompressor.decompressImage(compressedImage, blockSize);

            std::vector<uint8_t> decompressedData(originalData.size());
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    decompressedData[i * width + j] = static_cast<uint8_t>(std::round(decompressedImage[i][j]));
                }
            }

            bool isDecompressionCorrect = (originalData == decompressedData);
            if (isDecompressionCorrect) {
                std::cout << "Decompression successful." << std::endl;
            } else {
                std::cerr << "Decompression failed." << std::endl;
            }
            break;
        }
        //case 5: { // Fractal
            //FractalCompression fractalCompressor;
            //std::vector<double> pixelValuesDouble(originalData.size());
            //for (size_t i = 0; i < originalData.size(); ++i) {
            //    pixelValuesDouble[i] = static_cast<double>(originalData[i]);
            //}
            //std::vector<double> compressedData = fractalCompressor.compressFractal(pixelValuesDouble);
            //compressionRatio = static_cast<double>(compressedData.size() * sizeof(double)) / originalData.size();
            //break;
        }
        //default:
            //std::cerr << "Error: Invalid compression algorithm selected." << std::endl;
            //return 1;
    //}

    std::string outputFilePath = fs::path(inputFilePath).stem().string() + ".jpg";
    // добавить логику сжатых данных JPEG

    std::cout << "Compression ratio: " << compressionRatio * 100 << "%" << std::endl;
    std::cout << "Compressed file saved to: " << outputFilePath << std::endl;

    return 0;
}