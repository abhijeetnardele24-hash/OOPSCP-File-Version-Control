#include "../include/FileScanner.h"
#include "../include/FileUtils.h"
#include "../include/HashCalculator.h"
#include <iostream>
#include <windows.h>
#include <algorithm>

FileScanner::FileScanner() : rootPath(".") {
    // Add common ignore patterns
    ignoredDirs.insert(".oopscp");
    ignoredDirs.insert(".git");
    ignoredDirs.insert("node_modules");
}

FileScanner::FileScanner(const std::string& root) : rootPath(root) {
    ignoredDirs.insert(".oopscp");
    ignoredDirs.insert(".git");
    ignoredDirs.insert("node_modules");
}

void FileScanner::setRootPath(const std::string& root) {
    rootPath = root;
}

void FileScanner::addIgnorePattern(const std::string& pattern) {
    ignorePatterns.push_back(pattern);
}

void FileScanner::addIgnoreDirectory(const std::string& dirName) {
    ignoredDirs.insert(dirName);
}

void FileScanner::loadIgnoreFile(const std::string& ignoreFilePath) {
    if (!FileUtils::fileExists(ignoreFilePath)) return;
    
    std::ifstream file(ignoreFilePath);
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line[0] != '#') {
            ignorePatterns.push_back(line);
        }
    }
}

bool FileScanner::shouldIgnore(const std::string& path) const {
    // Check if path matches any ignore pattern
    for (const auto& pattern : ignorePatterns) {
        if (matchesPattern(path, pattern)) {
            return true;
        }
    }
    return false;
}

bool FileScanner::matchesPattern(const std::string& path, const std::string& pattern) const {
    // Simple wildcard matching (* means any characters)
    if (pattern.find('*') == std::string::npos) {
        return path.find(pattern) != std::string::npos;
    }
    
    // Check if pattern ends with the path
    if (pattern[0] == '*') {
        std::string suffix = pattern.substr(1);
        return path.length() >= suffix.length() &&
               path.substr(path.length() - suffix.length()) == suffix;
    }
    
    return false;
}

std::string FileScanner::getRelativePath(const std::string& fullPath) const {
    if (fullPath.find(rootPath) == 0) {
        std::string rel = fullPath.substr(rootPath.length());
        if (!rel.empty() && (rel[0] == '\\' || rel[0] == '/')) {
            rel = rel.substr(1);
        }
        return rel;
    }
    return fullPath;
}

std::vector<FileEntry> FileScanner::scanDirectory(bool recursive) {
    std::vector<FileEntry> entries;
    scanDirectoryRecursive(rootPath, entries, recursive);
    return entries;
}

void FileScanner::scanDirectoryRecursive(const std::string& dirPath, 
                                        std::vector<FileEntry>& entries, 
                                        bool recursive) {
    WIN32_FIND_DATAA findData;
    std::string searchPath = dirPath + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        std::string fileName = findData.cFileName;
        
        // Skip . and ..
        if (fileName == "." || fileName == "..") continue;
        
        std::string fullPath = dirPath + "\\" + fileName;
        std::string relPath = getRelativePath(fullPath);
        
        // Check if should ignore
        if (shouldIgnore(relPath)) continue;
        
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // It's a directory
            if (ignoredDirs.find(fileName) != ignoredDirs.end()) continue;
            
            if (recursive) {
                scanDirectoryRecursive(fullPath, entries, recursive);
            }
        } else {
            // It's a file
            long long size = ((long long)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
            
            // Convert FILETIME to time_t
            ULARGE_INTEGER ull;
            ull.LowPart = findData.ftLastWriteTime.dwLowDateTime;
            ull.HighPart = findData.ftLastWriteTime.dwHighDateTime;
            std::time_t modTime = ull.QuadPart / 10000000ULL - 11644473600ULL;
            
            // Calculate hash
            std::string hash = HashCalculator::calculateFileHash(fullPath);
            
            FileEntry entry(relPath, hash, size, modTime, false);
            entries.push_back(entry);
        }
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
}

std::vector<FileEntry> FileScanner::scanFiles(const std::vector<std::string>& paths) {
    std::vector<FileEntry> entries;
    
    for (const auto& path : paths) {
        if (FileUtils::fileExists(path)) {
            long long size = FileUtils::getFileSize(path);
            std::time_t modTime = FileUtils::getLastModifiedTime(path);
            std::string hash = HashCalculator::calculateFileHash(path);
            std::string relPath = getRelativePath(path);
            
            FileEntry entry(relPath, hash, size, modTime, false);
            entries.push_back(entry);
        }
    }
    
    return entries;
}

int FileScanner::getFileCount() const {
    FileScanner temp = *this;
    return temp.scanDirectory(true).size();
}

long long FileScanner::getTotalSize() const {
    FileScanner temp = *this;
    auto entries = temp.scanDirectory(true);
    long long total = 0;
    for (const auto& entry : entries) {
        total += entry.getSize();
    }
    return total;
}

void FileScanner::printScanResults(const std::vector<FileEntry>& entries) const {
    std::cout << "Scanned " << entries.size() << " files:" << std::endl;
    for (const auto& entry : entries) {
        std::cout << "  " << entry.getPath() 
                  << " (" << FileUtils::formatFileSize(entry.getSize()) << ")"
                  << std::endl;
    }
}
