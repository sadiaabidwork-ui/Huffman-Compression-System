#include "HuffmanEngine.h"
#include "BitStreamer.h"
#include <fstream>
#include <queue>
#include <stdexcept>

// Legacy compatibility macro for NULL pointers
#ifndef nullptr
#define nullptr NULL
#endif

HuffmanEngine::HuffmanEngine(char key) : cipher(key) {}

void HuffmanEngine::generateCodes(const Node* root, const std::string& str) {
    if (!root) return;
    if (root->isLeaf()) {
        huffmanCodes[static_cast<const LeafNode*>(root)->getData()] = str;
    } else {
        generateCodes(static_cast<const InternalNode*>(root)->getLeft(), str + "0");
        generateCodes(static_cast<const InternalNode*>(root)->getRight(), str + "1");
    }
}

void HuffmanEngine::serializeTree(const Node* root, std::string& treeStr) const {
    if (!root) return;
    if (root->isLeaf()) {
        treeStr += "1";
        treeStr += static_cast<const LeafNode*>(root)->getData();
    } else {
        treeStr += "0";
        serializeTree(static_cast<const InternalNode*>(root)->getLeft(), treeStr);
        serializeTree(static_cast<const InternalNode*>(root)->getRight(), treeStr);
    }
}

Node* HuffmanEngine::deserializeTree(const std::string& treeStr, int& index) {
    if (index >= static_cast<int>(treeStr.length())) {
        return nullptr;
    }
    if (treeStr[index] == '1') {
        if (index + 1 >= static_cast<int>(treeStr.length())) return nullptr;
        char ch = treeStr[++index];
        index++;
        return new LeafNode(ch, 0);
    } else {
        index++;
        Node* left = deserializeTree(treeStr, index);
        Node* right = deserializeTree(treeStr, index);
        return new InternalNode(left, right);
    }
}

void HuffmanEngine::compressFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inFile(inputFilePath.c_str(), std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Exception: Could not open the source file: " + inputFilePath);
    }

    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    if (content.empty()) {
        throw std::invalid_argument("Exception: Input file is empty. Compression aborted.");
    }

    std::map<char, int> frequencies;
    for (size_t i = 0; i < content.length(); ++i) {
        frequencies[content[i]]++;
    }

    std::priority_queue<Node*, std::vector<Node*>, NodePtrCompare> pq;
    for (std::map<char, int>::const_iterator it = frequencies.begin(); it != frequencies.end(); ++it) {
        pq.push(new LeafNode(it->first, it->second));
    }

    if (pq.empty()) {
        throw std::runtime_error("Exception: Frequency table initialization failed.");
    }

    // Edge-case: single unique character files handler
    if (pq.size() == 1) {
        Node* singleLeaf = pq.top(); pq.pop();
        pq.push(new InternalNode(singleLeaf, new LeafNode('\0', 0)));
    }

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new InternalNode(left, right));
    }

    Node* root = pq.top();
    huffmanCodes.clear();
    generateCodes(root, "");

    std::string treeMetaData = "";
    serializeTree(root, treeMetaData);

    BitStreamer<char> streamer;
    for (size_t i = 0; i < content.length(); ++i) {
        char ch = content[i];
        std::string code = huffmanCodes[ch];
        for (size_t j = 0; j < code.length(); ++j) {
            streamer << (code[j] == '1');
        }
    }
    streamer.flush();

    delete root; // Safely release memory allocation blocks

    std::string encodedBits(streamer.getBuffer().begin(), streamer.getBuffer().end());
    
    cipher.encryptDecrypt(treeMetaData);
    cipher.encryptDecrypt(encodedBits);

    std::ofstream outFile(outputFilePath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Exception: Target write path inaccessible: " + outputFilePath);
    }

    size_t treeSize = treeMetaData.size();
    size_t dataSize = encodedBits.size();

    outFile.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    outFile.write(treeMetaData.c_str(), treeSize);
    outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
    outFile.write(encodedBits.c_str(), dataSize);
    outFile.close();
}

void HuffmanEngine::decompressFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    std::ifstream inFile(inputFilePath.c_str(), std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Exception: Target encrypted archive path not found: " + inputFilePath);
    }

    size_t treeSize = 0, dataSize = 0;
    
    if (!inFile.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize))) {
        throw std::runtime_error("Exception: Corrupted archive header metadata.");
    }
    std::string treeMetaData(treeSize, '\0');
    inFile.read(&treeMetaData[0], treeSize);

    if (!inFile.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize))) {
        throw std::runtime_error("Exception: Corrupted archive internal structure.");
    }
    std::string encodedBits(dataSize, '\0');
    inFile.read(&encodedBits[0], dataSize);
    inFile.close();

    cipher.encryptDecrypt(treeMetaData);
    cipher.encryptDecrypt(encodedBits);

    int index = 0;
    Node* root = deserializeTree(treeMetaData, index);
    if (!root) {
        throw std::runtime_error("Exception: Reconstructing execution tree failed.");
    }

    std::ofstream outFile(outputFilePath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        delete root;
        throw std::runtime_error("Exception: Target extraction layout file write locked.");
    }

    Node* current = root;
    for (size_t i = 0; i < encodedBits.length(); ++i) {
        unsigned char byte = static_cast<unsigned char>(encodedBits[i]);
        for (int b = 7; b >= 0; --b) {
            bool bit = (byte >> b) & 1;
            
            if (bit) {
                current = static_cast<InternalNode*>(current)->getRight();
            } else {
                current = static_cast<InternalNode*>(current)->getLeft();
            }

            if (current->isLeaf()) {
                outFile << static_cast<LeafNode*>(current)->getData();
                current = root; 
            }
        }
    }

    delete root;
    outFile.close();
}
