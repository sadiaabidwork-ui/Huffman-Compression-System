#ifndef BITSTREAMER_H
#define BITSTREAMER_H

#include <vector>
#include <string>
#include <stdexcept>

template <typename T>
class BitStreamer {
private:
    std::vector<T> buffer;
    unsigned char currentByte;
    int bitCount;

public:
    BitStreamer() : currentByte(0), bitCount(0) {}

    // Overloaded Left Shift operator to stream bits in
    BitStreamer& operator<<(bool bit) {
        currentByte <<= 1;
        if (bit) {
            currentByte |= 1;
        }
        bitCount++;

        if (bitCount == 8) {
            buffer.push_back(static_cast<T>(currentByte));
            currentByte = 0;
            bitCount = 0;
        }
        return *this;
    }

    void flush() {
        if (bitCount > 0) {
            currentByte <<= (8 - bitCount);
            buffer.push_back(static_cast<T>(currentByte));
            currentByte = 0;
            bitCount = 0;
        }
    }

    const std::vector<T>& getBuffer() const { return buffer; }
    void clear() { buffer.clear(); currentByte = 0; bitCount = 0; }
};

#endif
