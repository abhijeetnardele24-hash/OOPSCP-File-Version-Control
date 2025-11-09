#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include <vector>

class Compressor {
private:
    // Run-Length Encoding helpers
    static std::vector<char> rleCompress(const std::vector<char>& data);
    static std::vector<char> rleDecompress(const std::vector<char>& data);
    
public:
    // Compress file
    static bool compressFile(const std::string& inputPath, const std::string& outputPath);
    
    // Decompress file
    static bool decompressFile(const std::string& inputPath, const std::string& outputPath);
    
    // Compress data in memory
    static std::vector<char> compress(const std::vector<char>& data);
    
    // Decompress data in memory
    static std::vector<char> decompress(const std::vector<char>& data);
    
    // Get compression ratio
    static double getCompressionRatio(long long originalSize, long long compressedSize);
    
    // Check if file is worth compressing (text files compress better)
    static bool isCompressible(const std::string& filePath);
    
    // Estimate compressed size without actually compressing
    static long long estimateCompressedSize(const std::string& filePath);
};

#endif
