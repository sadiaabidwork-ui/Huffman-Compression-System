#ifndef XORCIPHER_H
#define XORCIPHER_H

#include <string>

class XORCipher {
private:
    char key;

public:
    XORCipher(char encryptionKey = 'A');
    void encryptDecrypt(std::string& data) const;
};

#endif
