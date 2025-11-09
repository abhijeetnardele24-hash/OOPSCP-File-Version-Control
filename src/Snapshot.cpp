#include "../include/Snapshot.h"
#include "../include/HashCalculator.h"
#include "../include/FileUtils.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Snapshot::Snapshot() : timestamp(0), totalSize(0) {}

Snapshot::Snapshot(const std::string& message, const std::vector<FileEntry>& files)
    : message(message), files(files), totalSize(0) {
    timestamp = std::time(nullptr);
    calculateTotalSize();
    id = generateId();
}

std::string Snapshot::getId() const {
    return id;
}

std::string Snapshot::getMessage() const {
    return message;
}

std::time_t Snapshot::getTimestamp() const {
    return timestamp;
}

std::vector<FileEntry> Snapshot::getFiles() const {
    return files;
}

long long Snapshot::getTotalSize() const {
    return totalSize;
}

int Snapshot::getFileCount() const {
    return files.size();
}

void Snapshot::setId(const std::string& id) {
    this->id = id;
}

void Snapshot::setMessage(const std::string& message) {
    this->message = message;
}

void Snapshot::setTimestamp(std::time_t time) {
    this->timestamp = time;
}

void Snapshot::setFiles(const std::vector<FileEntry>& files) {
    this->files = files;
    calculateTotalSize();
}

void Snapshot::addFile(const FileEntry& file) {
    files.push_back(file);
    totalSize += file.getSize();
}

void Snapshot::calculateTotalSize() {
    totalSize = 0;
    for (const auto& file : files) {
        totalSize += file.getSize();
    }
}

std::string Snapshot::generateId() {
    // Generate ID based on timestamp and message
    std::ostringstream oss;
    oss << timestamp << message;
    std::string data = oss.str();
    
    // Get first 7 characters of hash
    std::string fullHash = HashCalculator::calculateHash(data);
    return fullHash.substr(0, 7);
}

bool Snapshot::saveToFile(const std::string& filePath) const {
    std::ofstream file(filePath, std::ios::binary);
    if (!file) return false;
    
    // Write magic number
    const char magic[] = "OOPSCP";
    file.write(magic, 6);
    
    // Write version
    int version = 1;
    file.write((char*)&version, sizeof(int));
    
    // Write ID
    int idLen = id.length();
    file.write((char*)&idLen, sizeof(int));
    file.write(id.c_str(), idLen);
    
    // Write message
    int msgLen = message.length();
    file.write((char*)&msgLen, sizeof(int));
    file.write(message.c_str(), msgLen);
    
    // Write timestamp
    file.write((char*)&timestamp, sizeof(std::time_t));
    
    // Write total size
    file.write((char*)&totalSize, sizeof(long long));
    
    // Write file count
    int fileCount = files.size();
    file.write((char*)&fileCount, sizeof(int));
    
    // Write each file entry
    for (const auto& entry : files) {
        entry.writeToBinary(file);
    }
    
    return true;
}

bool Snapshot::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) return false;
    
    // Read and verify magic number
    char magic[7] = {0};
    file.read(magic, 6);
    if (std::string(magic) != "OOPSCP") return false;
    
    // Read version
    int version;
    file.read((char*)&version, sizeof(int));
    
    // Read ID
    int idLen;
    file.read((char*)&idLen, sizeof(int));
    char* idBuf = new char[idLen + 1];
    file.read(idBuf, idLen);
    idBuf[idLen] = '\0';
    id = idBuf;
    delete[] idBuf;
    
    // Read message
    int msgLen;
    file.read((char*)&msgLen, sizeof(int));
    char* msgBuf = new char[msgLen + 1];
    file.read(msgBuf, msgLen);
    msgBuf[msgLen] = '\0';
    message = msgBuf;
    delete[] msgBuf;
    
    // Read timestamp
    file.read((char*)&timestamp, sizeof(std::time_t));
    
    // Read total size
    file.read((char*)&totalSize, sizeof(long long));
    
    // Read file count
    int fileCount;
    file.read((char*)&fileCount, sizeof(int));
    
    // Read file entries
    files.clear();
    for (int i = 0; i < fileCount; i++) {
        FileEntry entry;
        entry.readFromBinary(file);
        files.push_back(entry);
    }
    
    return true;
}

std::string Snapshot::toString() const {
    std::ostringstream oss;
    oss << "Snapshot " << id << ": " << message;
    return oss.str();
}

void Snapshot::printSummary() const {
    std::cout << "Snapshot ID: " << id << std::endl;
    std::cout << "Message: " << message << std::endl;
    std::cout << "Timestamp: " << FileUtils::formatTimestamp(timestamp) << std::endl;
    std::cout << "Files: " << files.size() << std::endl;
    std::cout << "Total Size: " << FileUtils::formatFileSize(totalSize) << std::endl;
}

void Snapshot::printDetailed() const {
    printSummary();
    std::cout << "\nFiles:" << std::endl;
    for (const auto& file : files) {
        std::cout << "  " << file.toString() << std::endl;
    }
}

const FileEntry* Snapshot::findFile(const std::string& path) const {
    for (const auto& file : files) {
        if (file.getPath() == path) {
            return &file;
        }
    }
    return nullptr;
}

FileEntry* Snapshot::getFile(const std::string& path) {
    for (auto& file : files) {
        if (file.getPath() == path) {
            return &file;
        }
    }
    return nullptr;
}

const FileEntry* Snapshot::getFile(const std::string& path) const {
    return findFile(path);
}

bool Snapshot::save(const std::string& filePath) const {
    return saveToFile(filePath);
}

bool Snapshot::load(const std::string& filePath) {
    return loadFromFile(filePath);
}
