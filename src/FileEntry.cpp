#include "../include/FileEntry.h"
#include "../include/FileUtils.h"
#include <cstring>

FileEntry::FileEntry() : size(0), modifiedTime(0), isDirectory(false) {}

FileEntry::FileEntry(const std::string& path, const std::string& hash, 
                     long long size, std::time_t modified, bool isDir)
    : relativePath(path), hash(hash), size(size), 
      modifiedTime(modified), isDirectory(isDir) {}

std::string FileEntry::getPath() const {
    return relativePath;
}

std::string FileEntry::getHash() const {
    return hash;
}

long long FileEntry::getSize() const {
    return size;
}

std::time_t FileEntry::getModifiedTime() const {
    return modifiedTime;
}

bool FileEntry::getIsDirectory() const {
    return isDirectory;
}

void FileEntry::setPath(const std::string& path) {
    relativePath = path;
}

void FileEntry::setHash(const std::string& h) {
    hash = h;
}

void FileEntry::setSize(long long s) {
    size = s;
}

void FileEntry::setModifiedTime(std::time_t modified) {
    modifiedTime = modified;
}

void FileEntry::setIsDirectory(bool isDir) {
    isDirectory = isDir;
}

void FileEntry::writeToBinary(std::ofstream& out) const {
    // Write path length and path
    int pathLen = relativePath.length();
    out.write((char*)&pathLen, sizeof(int));
    out.write(relativePath.c_str(), pathLen);
    
    // Write hash length and hash
    int hashLen = hash.length();
    out.write((char*)&hashLen, sizeof(int));
    out.write(hash.c_str(), hashLen);
    
    // Write size
    out.write((char*)&size, sizeof(long long));
    
    // Write modified time
    out.write((char*)&modifiedTime, sizeof(std::time_t));
    
    // Write isDirectory flag
    out.write((char*)&isDirectory, sizeof(bool));
}

void FileEntry::readFromBinary(std::ifstream& in) {
    // Read path
    int pathLen;
    in.read((char*)&pathLen, sizeof(int));
    char* pathBuf = new char[pathLen + 1];
    in.read(pathBuf, pathLen);
    pathBuf[pathLen] = '\0';
    relativePath = pathBuf;
    delete[] pathBuf;
    
    // Read hash
    int hashLen;
    in.read((char*)&hashLen, sizeof(int));
    char* hashBuf = new char[hashLen + 1];
    in.read(hashBuf, hashLen);
    hashBuf[hashLen] = '\0';
    hash = hashBuf;
    delete[] hashBuf;
    
    // Read size
    in.read((char*)&size, sizeof(long long));
    
    // Read modified time
    in.read((char*)&modifiedTime, sizeof(std::time_t));
    
    // Read isDirectory flag
    in.read((char*)&isDirectory, sizeof(bool));
}

std::string FileEntry::toString() const {
    return relativePath + " (" + FileUtils::formatFileSize(size) + ")";
}

bool FileEntry::operator==(const FileEntry& other) const {
    return relativePath == other.relativePath && hash == other.hash;
}

bool FileEntry::operator!=(const FileEntry& other) const {
    return !(*this == other);
}

bool FileEntry::contentChanged(const FileEntry& other) const {
    return hash != other.hash;
}
