#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "HuffmanEngine.h"

// Helper function to get the exact file size in bytes using standard file streams
long getFileSize(const std::string& filePath) {
    std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return -1; // Indicates file could not be read
    }
    long size = file.tellg();
    file.close();
    return size;
}

int main() {
    HuffmanEngine engine('X'); // 'X' serves as our encryption layer XOR key
    int choice = 0;
    std::string src, dest;

    while (choice != 3) {
        std::cout << "\n=============================================\n";
        std::cout << "=== Huffman Cryptographic Compressor Menu ===\n";
        std::cout << "=============================================\n";
        std::cout << "1. Encrypt & Compress File\n";
        std::cout << "2. Decrypt & Decompress File\n";
        std::cout << "3. Exit Program\n";
        std::cout << "Enter Choice (1-3): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string dummy;
            std::cin >> dummy;
            std::cout << "Invalid entry. Please input a numeric digit.\n";
            continue;
        }

        if (choice == 3) {
            std::cout << "\nClosing engine streams. Goodbye!\n";
            break;
        }

        if (choice != 1 && choice != 2) {
            std::cout << "Error: Selection option out of boundaries. Try again.\n";
            continue;
        }

        std::cout << "Enter Path to Source File: ";
        std::cin >> src;
        std::cout << "Enter Path to Output Target: ";
        std::cin >> dest;

        try {
            if (choice == 1) {
                std::cout << "\nExecuting structural parsing and secure compression operations...\n";
                
                // Get original size before processing
                long originalSize = getFileSize(src);
                
                engine.compressFile(src, dest);
                
                // Get compressed size after processing
                long compressedSize = getFileSize(dest);

                std::cout << "Process Complete!\n";
                std::cout << "---------------------------------------------\n";
                std::cout << "=== MEMORY OPTIMIZATION METRICS ===\n";
                if (originalSize != -1 && compressedSize != -1) {
                    std::cout << "[-] Original File Size  : " << originalSize << " Bytes\n";
                    std::cout << "[+] Compressed File Size: " << compressedSize << " Bytes\n";
                    
                    if (originalSize > 0) {
                        double savings = ((double)(originalSize - compressedSize) / originalSize) * 100.0;
                        // Format output to 2 decimal places smoothly
                        std::cout << "[*] Storage Space Saved : " << std::fixed << std::setprecision(2) << savings << "%\n";
                    }
                } else {
                    std::cout << "[!] Could not compute size metrics due to system file lock.\n";
                }
                std::cout << "---------------------------------------------\n";
                std::cout << "Data safely locked within: " << dest << "\n";
            } 
            else if (choice == 2) {
                std::cout << "\nValidating cryptographic signature and extracting data stream...\n";
                engine.decompressFile(src, dest);
                std::cout << "Process Complete! Restored uncompressed file at: " << dest << "\n";
            }
        } 
        catch (const std::exception& ex) {
            std::cerr << "\n[CRITICAL RUNTIME ERROR FLAG]\n" << ex.what() << "\n";
            std::cout << "Returning to main menu safely...\n";
        }
    }

    return 0;
}
