#ifndef HUFFMANENGINE_H
#define HUFFMANENGINE_H

#include <string>
#include <map>
#include "Node.h"
#include "XORCipher.h"

class HuffmanEngine {
private:
    std::map<char, std::string> huffmanCodes;
    XORCipher cipher;

    void generateCodes(const Node* root, const std::string& str);
    void serializeTree(const Node* root, std::string& treeStr) const;
    Node* deserializeTree(const std::string& treeStr, int& index);

public:
    HuffmanEngine(char key = 'K');
    
    // Core file handling operations carrying explicit exception management
    void compressFile(const std::string& inputFilePath, const std::string& outputFilePath);
    void decompressFile(const std::string& inputFilePath, const std::string& outputFilePath);
};

#endif
