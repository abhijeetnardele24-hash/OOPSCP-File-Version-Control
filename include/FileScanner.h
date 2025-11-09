#ifndef FILESCANNER_H
#define FILESCANNER_H

#include "FileEntry.h"
#include <string>
#include <vector>
#include <set>

class FileScanner {
private:
    std::string rootPath;                // Root directory to scan
    std::vector<std::string> ignorePatterns;  // Patterns to ignore (like .gitignore)
    std::set<std::string> ignoredDirs;   // Directories to skip
    
    // Helper functions
    bool shouldIgnore(const std::string& path) const;
    bool matchesPattern(const std::string& path, const std::string& pattern) const;
    std::string getRelativePath(const std::string& fullPath) const;
    void scanDirectoryRecursive(const std::string& dirPath, std::vector<FileEntry>& entries, bool recursive);
    
public:
    // Constructors
    FileScanner();
    FileScanner(const std::string& root);
    
    // Setters
    void setRootPath(const std::string& root);
    void addIgnorePattern(const std::string& pattern);
    void addIgnoreDirectory(const std::string& dirName);
    void loadIgnoreFile(const std::string& ignoreFilePath);  // Load .oopsignore file
    
    // Scanning
    std::vector<FileEntry> scanDirectory(bool recursive = true);
    std::vector<FileEntry> scanFiles(const std::vector<std::string>& paths);
    
    // Statistics
    int getFileCount() const;
    long long getTotalSize() const;
    
    // Display
    void printScanResults(const std::vector<FileEntry>& entries) const;
};

#endif
