#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <sys/stat.h>
#include <direct.h>
#include <io.h>

class FileUtils {
public:
    // Check if file exists
    static bool fileExists(const std::string& path);
    
    // Check if directory exists
    static bool directoryExists(const std::string& path);
    
    // Create directory
    static bool createDirectory(const std::string& path);
    
    // Get file size
    static long long getFileSize(const std::string& path);
    
    // Get last modified time
    static std::time_t getLastModifiedTime(const std::string& path);
    
    // Copy file
    static bool copyFile(const std::string& source, const std::string& dest);
    
    // Delete file
    static bool deleteFile(const std::string& path);
    
    // Read entire file content
    static std::string readFile(const std::string& path);
    
    // Write content to file
    static bool writeFile(const std::string& path, const std::string& content);
    
    // Read binary file
    static std::vector<char> readBinaryFile(const std::string& path);
    
    // Write binary file
    static bool writeBinaryFile(const std::string& path, const std::vector<char>& data);
    
    // Get file extension
    static std::string getFileExtension(const std::string& path);
    
    // Get filename without path
    static std::string getFileName(const std::string& path);
    
    // Format file size for display
    static std::string formatFileSize(long long bytes);
    
    // Format timestamp for display
    static std::string formatTimestamp(std::time_t timestamp);
    
    // Create directories recursively
    static bool createDirectories(const std::string& path);
};

#endif
