#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <string>
#include <ctime>
#include <fstream>

class FileEntry {
private:
    std::string relativePath;  // Relative path from repository root
    std::string hash;          // SHA256 hash of content
    long long size;            // File size in bytes
    std::time_t modifiedTime;  // Last modified timestamp
    bool isDirectory;          // Is this a directory?
    
public:
    // Constructors
    FileEntry();
    FileEntry(const std::string& path, const std::string& hash, long long size, std::time_t modified, bool isDir = false);
    
    // Getters
    std::string getPath() const;
    std::string getHash() const;
    long long getSize() const;
    std::time_t getModifiedTime() const;
    bool getIsDirectory() const;
    
    // Setters
    void setPath(const std::string& path);
    void setHash(const std::string& hash);
    void setSize(long long size);
    void setModifiedTime(std::time_t modified);
    void setIsDirectory(bool isDir);
    
    // Serialization for binary file storage
    void writeToBinary(std::ofstream& out) const;
    void readFromBinary(std::ifstream& in);
    
    // Display
    std::string toString() const;
    
    // Comparison
    bool operator==(const FileEntry& other) const;
    bool operator!=(const FileEntry& other) const;
    
    // Check if content changed (different hash)
    bool contentChanged(const FileEntry& other) const;
};

#endif
