#include "../include/HashCalculator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

// SHA-256 Constants
const unsigned int HashCalculator::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

unsigned int HashCalculator::rotr(unsigned int x, unsigned int n) {
    return (x >> n) | (x << (32 - n));
}

unsigned int HashCalculator::ch(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (~x & z);
}

unsigned int HashCalculator::maj(unsigned int x, unsigned int y, unsigned int z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

unsigned int HashCalculator::sigma0(unsigned int x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

unsigned int HashCalculator::sigma1(unsigned int x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

unsigned int HashCalculator::delta0(unsigned int x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

unsigned int HashCalculator::delta1(unsigned int x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

void HashCalculator::processBlock(const unsigned char* block, unsigned int* hash) {
    unsigned int W[64];
    unsigned int a, b, c, d, e, f, g, h, T1, T2;
    
    // Prepare message schedule
    for (int i = 0; i < 16; i++) {
        W[i] = (block[i * 4] << 24) | (block[i * 4 + 1] << 16) | 
               (block[i * 4 + 2] << 8) | block[i * 4 + 3];
    }
    
    for (int i = 16; i < 64; i++) {
        W[i] = delta1(W[i - 2]) + W[i - 7] + delta0(W[i - 15]) + W[i - 16];
    }
    
    // Initialize working variables
    a = hash[0]; b = hash[1]; c = hash[2]; d = hash[3];
    e = hash[4]; f = hash[5]; g = hash[6]; h = hash[7];
    
    // Main loop
    for (int i = 0; i < 64; i++) {
        T1 = h + sigma1(e) + ch(e, f, g) + K[i] + W[i];
        T2 = sigma0(a) + maj(a, b, c);
        h = g; g = f; f = e; e = d + T1;
        d = c; c = b; b = a; a = T1 + T2;
    }
    
    // Add to hash
    hash[0] += a; hash[1] += b; hash[2] += c; hash[3] += d;
    hash[4] += e; hash[5] += f; hash[6] += g; hash[7] += h;
}

std::string HashCalculator::calculateHash(const std::string& data) {
    unsigned int hash[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    unsigned char block[64];
    size_t dataLen = data.length();
    size_t bitLen = dataLen * 8;
    
    // Process full blocks
    size_t i = 0;
    while (i + 64 <= dataLen) {
        std::memcpy(block, data.c_str() + i, 64);
        processBlock(block, hash);
        i += 64;
    }
    
    // Prepare last block
    std::memset(block, 0, 64);
    size_t remaining = dataLen - i;
    std::memcpy(block, data.c_str() + i, remaining);
    block[remaining] = 0x80;
    
    // If not enough space for length, process this block and start new one
    if (remaining >= 56) {
        processBlock(block, hash);
        std::memset(block, 0, 64);
    }
    
    // Add length in bits
    for (int j = 0; j < 8; j++) {
        block[63 - j] = (bitLen >> (j * 8)) & 0xFF;
    }
    processBlock(block, hash);
    
    // Convert to hex string
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) {
        oss << std::hex << std::setw(8) << std::setfill('0') << hash[i];
    }
    return oss.str();
}

std::string HashCalculator::calculateFileHash(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return "";
    
    unsigned int hash[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    unsigned char block[64];
    long long totalBytes = 0;
    
    while (file.read((char*)block, 64)) {
        processBlock(block, hash);
        totalBytes += 64;
    }
    
    size_t lastBytes = file.gcount();
    totalBytes += lastBytes;
    
    // Padding
    std::memset(block + lastBytes, 0, 64 - lastBytes);
    block[lastBytes] = 0x80;
    
    if (lastBytes >= 56) {
        processBlock(block, hash);
        std::memset(block, 0, 64);
    }
    
    // Add length
    unsigned long long bitLen = totalBytes * 8;
    for (int j = 0; j < 8; j++) {
        block[63 - j] = (bitLen >> (j * 8)) & 0xFF;
    }
    processBlock(block, hash);
    
    // Convert to hex
    std::ostringstream oss;
    for (int i = 0; i < 8; i++) {
        oss << std::hex << std::setw(8) << std::setfill('0') << hash[i];
    }
    return oss.str();
}

bool HashCalculator::verifyFileHash(const std::string& filePath, const std::string& expectedHash) {
    std::string actualHash = calculateFileHash(filePath);
    return actualHash == expectedHash;
}

std::string HashCalculator::calculateBytesHash(const std::vector<char>& data) {
    std::string str(data.begin(), data.end());
    return calculateHash(str);
}
