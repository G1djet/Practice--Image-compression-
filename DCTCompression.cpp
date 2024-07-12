#include "DCTCompression.h"
#include <vector>
#include <cmath>
#include <corecrt_math_defines.h>

std::vector<double> ImageCompression::performDCT(const std::vector<double>& block) {
    int size = static_cast<int>(block.size());
    std::vector<double> dctBlock(size);

    for (int u = 0; u < size; ++u) {
        double sum = 0.0;
        for (int x = 0; x < size; ++x) {
            sum += block[x] * std::cos(M_PI * (2 * x + 1) * u / (2 * size));
        }
        dctBlock[u] = (u == 0) ? sum / std::sqrt(size) : sum * std::sqrt(2.0 / size);
    }

    return dctBlock;
}

std::vector<double> ImageCompression::performIDCT(const std::vector<double>& block) {
    int size = static_cast<int>(block.size());
    std::vector<double> idctBlock(size);

    for (int x = 0; x < size; ++x) {
        double sum = 0.0;
        for (int u = 0; u < size; ++u) {
            sum += block[u] * std::cos(M_PI * (2 * x + 1) * u / (2 * size));
        }
        if (x == 0) {
            idctBlock[x] = sum / std::sqrt(static_cast<double>(size));
        } else {
            idctBlock[x] = sum * std::sqrt(2.0 / static_cast<double>(size));
        }
    }

    return idctBlock;
}

std::vector<std::vector<double>> ImageCompression::compressImage(const std::vector<std::vector<double>>& image, int blockSize) {
    int rows = static_cast<int>(image.size());
    int cols = static_cast<int>(image[0].size());
    int numBlockRows = rows / blockSize;
    int numBlockCols = cols / blockSize;

    std::vector<std::vector<double>> compressedImage(numBlockRows * numBlockCols, std::vector<double>(blockSize * blockSize));

    for (int blockRow = 0; blockRow < numBlockRows; ++blockRow) {
        for (int blockCol = 0; blockCol < numBlockCols; ++blockCol) {
            std::vector<double> block(blockSize * blockSize);
            int index = 0;
            for (int y = blockRow * blockSize; y < (blockRow + 1) * blockSize; ++y) {
                for (int x = blockCol * blockSize; x < (blockCol + 1) * blockSize; ++x) {
                    block[index++] = image[y][x];
                }
            }
            std::vector<double> dctBlock = performDCT(block);
            int compressedIndex = blockRow * numBlockCols + blockCol;
            compressedImage[compressedIndex] = dctBlock;
        }
    }

    return compressedImage;
}

std::vector<std::vector<double>> ImageCompression::decompressImage(const std::vector<std::vector<double>>& compressedImage, int blockSize) {
    int numBlockRows = static_cast<int>(compressedImage.size()) / (compressedImage[0].size() / (blockSize * blockSize));
    int numBlockCols = compressedImage[0].size() / (blockSize * blockSize);
    int rows = numBlockRows * blockSize;
    int cols = numBlockCols * blockSize;

    std::vector<std::vector<double>> decompressedImage(rows, std::vector<double>(cols));

    for (int blockRow = 0; blockRow < numBlockRows; ++blockRow) {
        for (int blockCol = 0; blockCol < numBlockCols; ++blockCol) {
            std::vector<double> dctBlock = compressedImage[blockRow * numBlockCols + blockCol];
            std::vector<double> idctBlock = performIDCT(dctBlock);
            int index = 0;
            for (int y = blockRow * blockSize; y < (blockRow + 1) * blockSize; ++y) {
                for (int x = blockCol * blockSize; x < (blockCol + 1) * blockSize; ++x) {
                    decompressedImage[y][x] = idctBlock[index++];
                }
            }
        }
    }

    return decompressedImage;
}