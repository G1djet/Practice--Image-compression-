#include "FractalCompression.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <bitset>

FractalCompression::FractalCompression(const std::string& inputFile, const std::string& outputFile)
    : m_inputFile(inputFile), m_outputFile(outputFile) {}

void FractalCompression::compressBMP() {
    std::vector<double> pixelValues = getPixelValues(m_inputFile);
    std::vector<double> compressedData = compressFractal(pixelValues);
    writeCompressedData(m_outputFile, compressedData);
}

void FractalCompression::compressTIFF() {
    std::vector<double> pixelValues = getPixelValues(m_inputFile);
    std::vector<double> compressedData = compressFractal(pixelValues);
    writeCompressedData(m_outputFile, compressedData);
}

std::vector<double> FractalCompression::getPixelValues(const std::string& filename) {
    std::vector<double> pixelValues;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return pixelValues;
    }

    // еще добавить кусок в зависимости от формата
    // ...

    return pixelValues;
}

void FractalCompression::writeCompressedData(const std::string& filename, const std::vector<double>& compressedData) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Не удалось создать файл: " << filename << std::endl;
        return;
    }

    for (double value : compressedData) {
        outFile.write(reinterpret_cast<const char*>(&value), sizeof(double));
    }

    outFile.close();
}

std::vector<double> FractalCompression::compressFractal(const std::vector<double>& pixelValues) {
    std::vector<double> compressedData;

    int blockSize = 8;
    int width = static_cast<int>(sqrt(pixelValues.size()));
    int height = width;

    for (int y = 0; y < height; y += blockSize) {
        for (int x = 0; x < width; x += blockSize) {
            std::vector<double> block;
            for (int j = y; j < y + blockSize; ++j) {
                for (int i = x; i < x + blockSize; ++i) {
                    block.push_back(pixelValues[j * width + i]);
                }
            }

            double similarity = 0.0;
            int offsetX = 0, offsetY = 0;
            double scale = 1.0;
            double rotation = 0.0;

            compressedData.push_back(similarity);
            compressedData.push_back(offsetX);
            compressedData.push_back(offsetY);
            compressedData.push_back(scale);
            compressedData.push_back(rotation);
        }
    }

    return compressedData;
}