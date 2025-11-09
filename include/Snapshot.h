#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "FileEntry.h"
#include <string>
#include <vector>
#include <ctime>
#include <map>

class Snapshot {
private:
    std::string id;                      // Unique snapshot ID (hash of content)
    std::string message;                 // Snapshot description
    std::time_t timestamp;               // When snapshot was created
    std::vector<FileEntry> files;        // All files in this snapshot
    long long totalSize;                 // Total size of all files
    
public:
    // Constructors
    Snapshot();
    Snapshot(const std::string& message, const std::vector<FileEntry>& files);
    
    // Getters
    std::string getId() const;
    std::string getMessage() const;
    std::time_t getTimestamp() const;
    std::vector<FileEntry> getFiles() const;
    long long getTotalSize() const;
    int getFileCount() const;
    
    // Setters
    void setId(const std::string& id);
    void setMessage(const std::string& message);
    void setTimestamp(std::time_t time);
    void setFiles(const std::vector<FileEntry>& files);
    
    // Operations
    void addFile(const FileEntry& file);
    void calculateTotalSize();
    std::string generateId();  // Generate unique ID based on content
    
    // Serialization
    bool saveToFile(const std::string& filePath) const;
    bool loadFromFile(const std::string& filePath);
    
    // Display
    std::string toString() const;
    void printSummary() const;
    void printDetailed() const;
    
    // Find file by path
    const FileEntry* findFile(const std::string& path) const;
    FileEntry* getFile(const std::string& path);
    const FileEntry* getFile(const std::string& path) const;
    
    // New save/load methods
    bool save(const std::string& filePath) const;
    bool load(const std::string& filePath);
};

#endif
