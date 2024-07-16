#include "FractalCompression.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <cstdint>

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

void readBMPHeader(std::ifstream& inputFile, int& width, int& height, int& bitsPerPixel) {
    // Структура заголовка BMP
    struct BMPHeader {
        uint16_t bfType;      
        uint32_t bfSize;     
        uint16_t bfReserved1; //  должно быть 0
        uint16_t bfReserved2; //  должно быть 0
        uint32_t bfOffBits;   
        uint32_t biSize;     
        int32_t  biWidth;     
        int32_t  biHeight;    
        uint16_t biPlanes;    
        uint16_t biBitCount;  
        uint32_t biCompression; 
        uint32_t biSizeImage;   
        int32_t  biXPelsPerMeter; 
        int32_t  biYPelsPerMeter; 
        uint32_t biClrUsed;   
        uint32_t biClrImportant; 
    };

    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    width = header.biWidth;
    height = header.biHeight;
    bitsPerPixel = header.biBitCount;

    if (header.bfType != 0x4D42) { 
        throw std::runtime_error("Неверный формат BMP файла");
    }
}

std::vector<double> FractalCompression::getPixelValues(const std::string& filename) {
    std::vector<double> pixelValues;
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
        return pixelValues;
    }

    char header[2];
    inputFile.read(header, 2);
    inputFile.seekg(0, std::ios::beg);

    if (header[0] == 'B' && header[1] == 'M') {
        int width, height, bitsPerPixel;
        readBMPHeader(inputFile, width, height, bitsPerPixel);
        readBMPPixels(inputFile, pixelValues, width, height, bitsPerPixel);
    } else if (header[0] == 'M' && header[1] == 'M' || header[0] == 'I' && header[1] == 'I') {
        int width, height, bitsPerPixel;
        readTIFFHeader(inputFile, width, height, bitsPerPixel);
        readTIFFPixels(inputFile, pixelValues, width, height, bitsPerPixel);
    } else {
        std::cerr << "Неизвестный формат файла: " << filename << std::endl;
    }

    inputFile.close();
    return pixelValues;
}


void readBMPPixels(std::ifstream& inputFile, std::vector<double>& pixelValues, int width, int height, int bitsPerPixel) {
    int rowStride = (width * (bitsPerPixel / 8) + 3) / 4 * 4;

    int padding = rowStride - (width * (bitsPerPixel / 8));

    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            double pixelValue;
            if (bitsPerPixel == 24) {
                uint8_t blue, green, red;
                inputFile.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));
                pixelValue = (red + green + blue) / 3.0;
            } else if (bitsPerPixel == 8) {
                uint8_t grayValue;
                inputFile.read(reinterpret_cast<char*>(&grayValue), sizeof(uint8_t));
                pixelValue = static_cast<double>(grayValue);
            } else {
                throw std::runtime_error("Неподдерживаемая глубина цвета в BMP файле");
            }
            pixelValues.push_back(pixelValue);
        }
        inputFile.ignore(padding);
    }
}

void readTIFFHeader(std::ifstream& inputFile, int& width, int& height, int& bitsPerPixel) {
    struct TIFFHeader {
        uint16_t byteOrder;    
        uint16_t version;      
        uint32_t offsetIFD;    
    };

    struct IFDEntry {
        uint16_t tag;          
        uint16_t type;         
        uint32_t count;        
        uint32_t valueOrOffset; 
    };

    TIFFHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(TIFFHeader));

    if (header.byteOrder == 0x4D4D) { 
        swapEndian(header.version);
        swapEndian(header.offsetIFD);
    } else if (header.byteOrder != 0x4949) { 
        throw std::runtime_error("Неподдерживаемый порядок байт в TIFF файле");
    }

    inputFile.seekg(header.offsetIFD, std::ios::beg);

    uint16_t numEntries;
    inputFile.read(reinterpret_cast<char*>(&numEntries), sizeof(uint16_t));
    if (header.byteOrder == 0x4D4D) {
        swapEndian(numEntries);
    }

    for (uint16_t i = 0; i < numEntries; ++i) {
        IFDEntry entry;
        inputFile.read(reinterpret_cast<char*>(&entry), sizeof(IFDEntry));
        if (header.byteOrder == 0x4D4D) {
            swapEndian(entry.tag);
            swapEndian(entry.type);
            swapEndian(entry.count);
            swapEndian(entry.valueOrOffset);
        }

        if (entry.tag == 0x0100) { 
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&width), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(width);
            }
        } else if (entry.tag == 0x0101) { 
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&height), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(height);
            }
        } else if (entry.tag == 0x0102) { 
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&bitsPerPixel), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(bitsPerPixel);
            }
        }
    }
}

template <typename T>
void swapEndian(T& value) {
    T swapped = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        swapped = (swapped << 8) | (value & 0xFF);
        value >>= 8;
    }
    value = swapped;
}

void readTIFFPixels(std::ifstream& inputFile, std::vector<double>& pixelValues, int width, int height, int bitsPerPixel) {
    // добавить чтение пикселей
    // хз почему я это да этого не написала
}
void readBMPHeader(std::ifstream& inputFile, int& width, int& height, int& bitsPerPixel) {
    inputFile.ignore(18);

    inputFile.read(reinterpret_cast<char*>(&width), sizeof(int));
    inputFile.read(reinterpret_cast<char*>(&height), sizeof(int));

    inputFile.ignore(2);

    uint16_t bitCount;
    inputFile.read(reinterpret_cast<char*>(&bitCount), sizeof(uint16_t));
    bitsPerPixel = static_cast<int>(bitCount);
}

void readBMPPixels(std::ifstream& inputFile, std::vector<double>& pixelValues, int width, int height, int bitsPerPixel) {
    int padding = (4 - ((width * (bitsPerPixel / 8)) % 4)) % 4;
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            double pixelValue;
            if (bitsPerPixel == 24) {
                uint8_t blue, green, red;
                inputFile.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));
                pixelValue = (red + green + blue) / 3.0;
            } else if (bitsPerPixel == 8) {
                uint8_t grayValue;
                inputFile.read(reinterpret_cast<char*>(&grayValue), sizeof(uint8_t));
                pixelValue = static_cast<double>(grayValue);
            } else {
            }
            pixelValues.push_back(pixelValue);
        }
        inputFile.ignore(padding);
    }
}

void readTIFFHeader(std::ifstream& inputFile, int& width, int& height, int& bitsPerPixel) {
    struct TIFFHeader {
        uint16_t byteOrder;    
        uint16_t version;      
        uint32_t offsetIFD;    
    };

    struct IFDEntry {
        uint16_t tag;         
        uint16_t type;         
        uint32_t count;        
        uint32_t valueOrOffset; 
    };

    TIFFHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(TIFFHeader));

    if (header.byteOrder == 0x4D4D) { 
        swapEndian(header.version);
        swapEndian(header.offsetIFD);
    } else if (header.byteOrder != 0x4949) { 
        throw std::runtime_error("Неподдерживаемый порядок байт в TIFF файле");
    }

    inputFile.seekg(header.offsetIFD, std::ios::beg);

    uint16_t numEntries;
    inputFile.read(reinterpret_cast<char*>(&numEntries), sizeof(uint16_t));
    if (header.byteOrder == 0x4D4D) {
        swapEndian(numEntries);
    }

    for (uint16_t i = 0; i < numEntries; ++i) {
        IFDEntry entry;
        inputFile.read(reinterpret_cast<char*>(&entry), sizeof(IFDEntry));
        if (header.byteOrder == 0x4D4D) {
            swapEndian(entry.tag);
            swapEndian(entry.type);
            swapEndian(entry.count);
            swapEndian(entry.valueOrOffset);
        }

        if (entry.tag == 0x0100) { // Ширина
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&width), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(width);
            }
        } else if (entry.tag == 0x0101) { // Высота
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&height), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(height);
            }
        } else if (entry.tag == 0x0102) { // Глубина цвета
            inputFile.seekg(entry.valueOrOffset, std::ios::beg);
            inputFile.read(reinterpret_cast<char*>(&bitsPerPixel), sizeof(int));
            if (header.byteOrder == 0x4D4D) {
                swapEndian(bitsPerPixel);
            }
        }
    }
}

template <typename T>
void swapEndian(T& value) {
    T swapped = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        swapped = (swapped << 8) | (value & 0xFF);
        value >>= 8;
    }
    value = swapped;
}

void readTIFFPixels(std::ifstream& inputFile, std::vector<double>& pixelValues, int width, int height, int bitsPerPixel) {
    // Вычисление количества байт на строку с учетом выравнивания
    int rowStride = (width * (bitsPerPixel / 8) + 3) / 4 * 4;

    // Чтение пикселей
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            double pixelValue;
            if (bitsPerPixel == 24) {
                // 24-битный цвет (RGB)
                uint8_t blue, green, red;
                inputFile.read(reinterpret_cast<char*>(&blue), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&green), sizeof(uint8_t));
                inputFile.read(reinterpret_cast<char*>(&red), sizeof(uint8_t));
                pixelValue = (red + green + blue) / 3.0;
            } else if (bitsPerPixel == 8) {
                // 8-битный оттенок серого
                uint8_t grayValue;
                inputFile.read(reinterpret_cast<char*>(&grayValue), sizeof(uint8_t));
                pixelValue = static_cast<double>(grayValue);
            } else if (bitsPerPixel == 16) {
                // 16-битный оттенок серого
                uint16_t grayValue;
                inputFile.read(reinterpret_cast<char*>(&grayValue), sizeof(uint16_t));
                pixelValue = static_cast<double>(grayValue);
            } else {
                // Обработка других глубин цвета
                throw std::runtime_error("Неподдерживаемая глубина цвета в TIFF файле");
            }
            pixelValues.push_back(pixelValue);
        }
        // Пропуск выравнивающих байт в конце строки
        inputFile.ignore(rowStride - (width * (bitsPerPixel / 8)));
    }
}


void FractalCompression::writeCompressedData(const std::string& filename, const std::vector<double>& compressedData) {
    // Реализация функции для записи сжатых данных в выходной файл
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Не удалось создать файл: " << filename << std::endl;
        return;
    }

    // Код для записи сжатых данных в файл
    for (double value : compressedData) {
        outFile.write(reinterpret_cast<const char*>(&value), sizeof(double));
    }

    outFile.close();
}

double computeSimilarity(
    const std::vector<double>& pixelValues,
    int width, int height,
    int blockX, int blockY, int blockSize,
    int offsetX, int offsetY,
    double scale, double rotation,
    const std::vector<std::vector<uint8_t>>& dictionary
) {
    int dictionaryBlockSize = dictionary[0].size() / (blockSize * blockSize);
    int dictionaryBlockX = 0, dictionaryBlockY = 0;

    // Преобразование блока исходного изображения
    std::vector<double> transformedBlock(blockSize * blockSize);
    for (int y = 0; y < blockSize; ++y) {
        for (int x = 0; x < blockSize; ++x) {
            int sourceX = blockX * blockSize + x + offsetX;
            int sourceY = blockY * blockSize + y + offsetY;
            if (sourceX >= 0 && sourceX < width && sourceY >= 0 && sourceY < height) {
                int index = (sourceY * width + sourceX);
                transformedBlock[y * blockSize + x] = pixelValues[index];
            } else {
                transformedBlock[y * blockSize + x] = 0.0;
            }
        }
    }

    double bestSimilarity = 0.0;
    for (int dictionaryBlockY = 0; dictionaryBlockY < dictionaryBlockSize; ++dictionaryBlockY) {
        for (int dictionaryBlockX = 0; dictionaryBlockX < dictionaryBlockSize; ++dictionaryBlockX) {
            double similarity = 0.0;
            for (int y = 0; y < blockSize; ++y) {
                for (int x = 0; x < blockSize; ++x) {
                    int dictionaryIndex = (dictionaryBlockY * blockSize + y) * dictionaryBlockSize * blockSize + (dictionaryBlockX * blockSize + x);
                    int transformedIndex = y * blockSize + x;
                    similarity += abs(transformedBlock[transformedIndex] - static_cast<double>(dictionary[dictionaryBlockY][dictionaryIndex]));
                }
            }
            similarity = 1.0 - similarity / (blockSize * blockSize * 255.0);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
            }
        }
    }

    return bestSimilarity;
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

            int numBlocksX = (width + blockSize - 1) / blockSize;
            int numBlocksY = (height + blockSize - 1) / blockSize;

            const double M_PI = 3.14159265358979323846;/// пока так, по другому она не определяется и с corecrt_math_defines.h тож какие-то проблемы
            int maxOffset = 8; // Максимальное смещение блока
            double minScale = 0.8; // Минимальный масштаб
            double maxScale = 1.2; // Максимальный масштаб
            double scaleStep = 0.05; // Шаг изменения масштаба
            double maxRotation = M_PI / 6; // Максимальный угол вращения (30 градусов)
            double rotationStep = M_PI / 36; // Шаг изменения вращения (5 градусов)

            // Предполагается, что "dictionary" является вектором векторов uint8_t, представляющих блоки изображения(но мало ли что там предполагается)
            std::vector<std::vector<uint8_t>> dictionary;
            int numBlocks = (width / blockSize) * (height / blockSize);
            dictionary.resize(numBlocks);

            int blockIndex = 0;
            std::vector<uint8_t> pixelValues(width * height); // замените на ваши пиксельные значения
            for (int y = 0; y < height; y += blockSize) {
                for (int x = 0; x < width; x += blockSize) {
                    for (int j = y; j < y + blockSize; ++j) {
                        for (int i = x; i < x + blockSize; ++i) {
                            dictionary[blockIndex].push_back(static_cast<uint8_t>(pixelValues[j * width + i]));
                        }
                    }
                    ++blockIndex;
                }
            }            

            for (int blockY = 0; blockY < numBlocksY; ++blockY) {
                for (int blockX = 0; blockX < numBlocksX; ++blockX) {
                    double bestSimilarity = 0.0;
                    int bestOffsetX = 0, bestOffsetY = 0;
                    double bestScale = 1.0;
                    double bestRotation = 0.0;

                    for (int offsetXCandidate = -maxOffset; offsetXCandidate <= maxOffset; ++offsetXCandidate) {
                        for (int offsetYCandidate = -maxOffset; offsetYCandidate <= maxOffset; ++offsetYCandidate) {
                            for (double scaleCandidate = minScale; scaleCandidate <= maxScale; scaleCandidate += scaleStep) {
                                for (double rotationCandidate = -maxRotation; rotationCandidate <= maxRotation; rotationCandidate += rotationStep) {
                                    double similarity = computeSimilarity(
                                        pixelValues,
                                        width, height,
                                        blockX, blockY, blockSize,
                                        offsetXCandidate, offsetYCandidate,
                                        scaleCandidate, rotationCandidate,
                                        dictionary
                                    );

                                    if (similarity > bestSimilarity) {
                                        bestSimilarity = similarity;
                                        bestOffsetX = offsetXCandidate;
                                        bestOffsetY = offsetYCandidate;
                                        bestScale = scaleCandidate;
                                        bestRotation = rotationCandidate;
                                    }
                                }
                            }
                        }
                    }

                    similarity += bestSimilarity;
                    offsetX = bestOffsetX;
                    offsetY = bestOffsetY;
                    scale = bestScale;
                    rotation = bestRotation;

                    compressedData.push_back(bestSimilarity);
                    compressedData.push_back(static_cast<double>(bestOffsetX));
                    compressedData.push_back(static_cast<double>(bestOffsetY));
                    compressedData.push_back(bestScale);
                    compressedData.push_back(bestRotation);
                }
            }

            return compressedData;
        }
    }
}