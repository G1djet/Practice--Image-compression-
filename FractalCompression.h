#ifndef FRACTAL_COMPRESSION_H
#define FRACTAL_COMPRESSION_H

#include <string>
#include <vector>

class FractalCompression {
public:
    FractalCompression(const std::string& inputFile, const std::string& outputFile);
    void compressBMP();
    void compressTIFF();

private:
    std::string m_inputFile;
    std::string m_outputFile;

    // Вспомогательные функции
    std::vector<double> getPixelValues(const std::string& filename);
    void writeCompressedData(const std::string& filename, const std::vector<double>& compressedData);
    std::vector<double> compressFractal(const std::vector<double>& pixelValues);
};

#endif // FRACTAL_COMPRESSION_H