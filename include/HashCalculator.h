#ifndef HASHCALCULATOR_H
#define HASHCALCULATOR_H

#include <string>
#include <vector>

class HashCalculator {
private:
    // SHA256 constants
    static const unsigned int K[64];
    
    // Helper functions for SHA256
    static unsigned int rotr(unsigned int x, unsigned int n);
    static unsigned int ch(unsigned int x, unsigned int y, unsigned int z);
    static unsigned int maj(unsigned int x, unsigned int y, unsigned int z);
    static unsigned int sigma0(unsigned int x);
    static unsigned int sigma1(unsigned int x);
    static unsigned int delta0(unsigned int x);
    static unsigned int delta1(unsigned int x);
    
    // Process message block
    static void processBlock(const unsigned char* block, unsigned int* hash);
    
public:
    // Calculate SHA256 hash of a string
    static std::string calculateHash(const std::string& data);
    
    // Calculate SHA256 hash of a file
    static std::string calculateFileHash(const std::string& filePath);
    
    // Verify file integrity
    static bool verifyFileHash(const std::string& filePath, const std::string& expectedHash);
    
    // Calculate hash for byte array
    static std::string calculateBytesHash(const std::vector<char>& data);
};

#endif
