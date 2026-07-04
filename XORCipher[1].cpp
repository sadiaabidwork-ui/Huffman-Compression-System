#include "XORCipher.h"

XORCipher::XORCipher(char encryptionKey) : key(encryptionKey) {}

void XORCipher::encryptDecrypt(std::string& data) const {
    for (size_t i = 0; i < data.length(); ++i) {
        data[i] ^= key; // Lightweight, precise bitwise manipulation
    }
}
