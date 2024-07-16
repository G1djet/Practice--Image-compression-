#include "Huffman.h"
#include <queue>
#include <bitset>

HuffmanNode::HuffmanNode(uint8_t val, int freq, HuffmanNode *l, HuffmanNode *r)
    : value(val), frequency(freq), left(l), right(r) {}

bool HuffmanNode::operator>(const HuffmanNode& other) const {
    return frequency > other.frequency;
}

void buildHuffmanCode(HuffmanNode* root, std::unordered_map<uint8_t, std::string>& codes, std::string code) {
    if (root->left == nullptr && root->right == nullptr) {
        codes[root->value] = code;
        return;
    }

    buildHuffmanCode(root->left, codes, code + "0");
    buildHuffmanCode(root->right, codes, code + "1");
}

std::vector<uint8_t> compressHuffman(const std::vector<uint8_t>& pixelData) {
    std::unordered_map<uint8_t, int> frequencyMap;
    for (uint8_t pixel : pixelData) {
        frequencyMap[pixel]++;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, std::greater<HuffmanNode*>> pq;
    for (const auto& [value, freq] : frequencyMap) {
        pq.push(new HuffmanNode(value, freq));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        pq.push(new HuffmanNode(0, left->frequency + right->frequency, left, right));
    }

    HuffmanNode* root = pq.top();

    std::unordered_map<uint8_t, std::string> huffmanCodes;
    buildHuffmanCode(root, huffmanCodes);

    std::vector<uint8_t> compressedData;
    std::string bitstream;
    for (uint8_t pixel : pixelData) {
        bitstream += huffmanCodes[pixel];
    }

    while (bitstream.length() % 8 != 0) {
        bitstream += "0";
    }

    for (size_t i = 0; i < bitstream.length(); i += 8) {
        std::bitset<8> byte(bitstream.substr(i, 8));
        compressedData.push_back(static_cast<uint8_t>(byte.to_ulong()));
    }

    return compressedData;
}

std::vector<uint8_t> decompressHuffman(const std::vector<uint8_t>& compressedData, const std::unordered_map<uint8_t, std::string>& huffmanCodes) {
    std::vector<uint8_t> decompressedData;
    std::string bitstream;
    for (uint8_t byte : compressedData) {
        bitstream += std::bitset<8>(byte).to_string();
    }

    std::string currentCode;
    for (char bit : bitstream) {
        currentCode += bit;
        for (const auto& [value, code] : huffmanCodes) {
            if (code == currentCode) {
                decompressedData.push_back(value);
                currentCode.clear();
                break;
            }
        }
    }

    return decompressedData;
}

HuffmanNode* buildHuffmanTree(const std::vector<uint8_t>& data) {
    std::unordered_map<uint8_t, int> frequencyMap;
    for (uint8_t byte : data) {
        frequencyMap[byte]++;
    }

    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, std::greater<HuffmanNode*>> pq;
    for (const auto& [value, freq] : frequencyMap) {
        pq.push(new HuffmanNode(value, freq));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();
        pq.push(new HuffmanNode(0, left->frequency + right->frequency, left, right));
    }

    return pq.top();
}