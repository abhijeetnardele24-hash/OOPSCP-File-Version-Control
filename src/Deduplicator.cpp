#include "../include/Deduplicator.h"
#include "../include/FileScanner.h"
#include "../include/FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

std::string DuplicateGroup::toString() const {
    std::stringstream ss;
    ss << "Hash: " << hash.substr(0, 16) << "..." << std::endl;
    ss << "Count: " << filePaths.size() << " files" << std::endl;
    ss << "Size per file: " << FileUtils::formatFileSize(fileSize) << std::endl;
    ss << "Wasted space: " << FileUtils::formatFileSize(wastedSpace) << std::endl;
    ss << "Files:" << std::endl;
    for (const auto& path : filePaths) {
        ss << "  - " << path << std::endl;
    }
    return ss.str();
}

Deduplicator::Deduplicator() : repositoryPath("."), totalWastedSpace(0) {}

Deduplicator::Deduplicator(const std::string& repoPath) 
    : repositoryPath(repoPath), totalWastedSpace(0) {}

void Deduplicator::buildHashMap(const std::vector<FileEntry>& files) {
    duplicateGroups.clear();
    totalWastedSpace = 0;
    
    // Build map of hash -> files
    std::map<std::string, std::vector<FileEntry>> hashMap;
    
    for (const auto& file : files) {
        hashMap[file.getHash()].push_back(file);
    }
    
    // Find duplicates (hash with more than one file)
    for (const auto& pair : hashMap) {
        if (pair.second.size() > 1) {
            const std::string& hash = pair.first;
            long long fileSize = pair.second[0].getSize();
            
            DuplicateGroup group(hash, fileSize);
            for (const auto& file : pair.second) {
                group.filePaths.push_back(file.getPath());
            }
            group.calculateWastedSpace();
            
            totalWastedSpace += group.wastedSpace;
            duplicateGroups.insert(std::make_pair(hash, group));
        }
    }
}

void Deduplicator::scanForDuplicates() {
    FileScanner scanner(repositoryPath);
    scanner.loadIgnoreFile(repositoryPath + "\\.oopsignore");
    auto files = scanner.scanDirectory(true);
    
    buildHashMap(files);
}

void Deduplicator::scanDirectory(const std::string& dirPath) {
    FileScanner scanner(dirPath);
    auto files = scanner.scanDirectory(true);
    
    buildHashMap(files);
}

std::vector<DuplicateGroup> Deduplicator::getDuplicateGroups() const {
    std::vector<DuplicateGroup> groups;
    for (const auto& pair : duplicateGroups) {
        groups.push_back(pair.second);
    }
    
    // Sort by wasted space (descending)
    std::sort(groups.begin(), groups.end(), 
              [](const DuplicateGroup& a, const DuplicateGroup& b) {
                  return a.wastedSpace > b.wastedSpace;
              });
    
    return groups;
}

int Deduplicator::getDuplicateFileCount() const {
    int count = 0;
    for (const auto& pair : duplicateGroups) {
        count += pair.second.filePaths.size();
    }
    return count;
}

long long Deduplicator::getTotalWastedSpace() const {
    return totalWastedSpace;
}

void Deduplicator::printDuplicateSummary() const {
    if (duplicateGroups.empty()) {
        std::cout << "No duplicate files found." << std::endl;
        return;
    }
    
    std::cout << "=== Duplicate File Summary ===" << std::endl;
    std::cout << "Duplicate groups: " << duplicateGroups.size() << std::endl;
    std::cout << "Total duplicate files: " << getDuplicateFileCount() << std::endl;
    std::cout << "Wasted space: " << FileUtils::formatFileSize(totalWastedSpace) << std::endl;
    std::cout << std::endl;
}

void Deduplicator::printDuplicateDetails() const {
    if (duplicateGroups.empty()) {
        std::cout << "No duplicate files found." << std::endl;
        return;
    }
    
    auto groups = getDuplicateGroups();
    
    std::cout << "=== Duplicate Files ===" << std::endl;
    std::cout << "Found " << groups.size() << " groups of duplicates" << std::endl;
    std::cout << std::endl;
    
    int groupNum = 1;
    for (const auto& group : groups) {
        std::cout << "Group " << groupNum++ << ":" << std::endl;
        std::cout << group.toString() << std::endl;
    }
}

bool Deduplicator::removeDuplicates(bool keepFirst) {
    if (duplicateGroups.empty()) {
        std::cout << "No duplicates to remove." << std::endl;
        return false;
    }
    
    int removedCount = 0;
    long long freedSpace = 0;
    
    for (const auto& pair : duplicateGroups) {
        const auto& group = pair.second;
        
        // Keep first file, remove others
        for (size_t i = (keepFirst ? 1 : 0); i < group.filePaths.size(); i++) {
            const std::string& path = group.filePaths[i];
            if (FileUtils::deleteFile(path)) {
                removedCount++;
                freedSpace += group.fileSize;
                std::cout << "Removed: " << path << std::endl;
            }
        }
    }
    
    std::cout << "\nRemoved " << removedCount << " duplicate files" << std::endl;
    std::cout << "Freed space: " << FileUtils::formatFileSize(freedSpace) << std::endl;
    
    return true;
}

bool Deduplicator::createHardLinks(const std::string& hash) {
    auto it = duplicateGroups.find(hash);
    if (it == duplicateGroups.end()) {
        std::cout << "No duplicate group found for hash." << std::endl;
        return false;
    }
    
    // Hard link creation is Windows-specific and requires admin privileges
    std::cout << "Hard link creation not implemented yet." << std::endl;
    return false;
}

void Deduplicator::exportToFile(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    
    if (!file.is_open()) {
        std::cout << "Error: Could not open output file." << std::endl;
        return;
    }
    
    file << "Duplicate File Report" << std::endl;
    file << "=====================" << std::endl;
    file << "Duplicate groups: " << duplicateGroups.size() << std::endl;
    file << "Total duplicate files: " << getDuplicateFileCount() << std::endl;
    file << "Wasted space: " << FileUtils::formatFileSize(totalWastedSpace) << std::endl;
    file << std::endl;
    
    auto groups = getDuplicateGroups();
    int groupNum = 1;
    
    for (const auto& group : groups) {
        file << "Group " << groupNum++ << ":" << std::endl;
        file << group.toString() << std::endl;
    }
    
    file.close();
    std::cout << "Report exported to: " << outputPath << std::endl;
}

Deduplicator::Stats Deduplicator::getStatistics() const {
    Stats stats;
    stats.duplicateFiles = getDuplicateFileCount();
    stats.uniqueFiles = duplicateGroups.size();
    stats.totalFiles = stats.duplicateFiles;
    stats.wastedSpace = totalWastedSpace;
    
    // Calculate total size
    stats.totalSize = 0;
    for (const auto& pair : duplicateGroups) {
        stats.totalSize += pair.second.fileSize * pair.second.filePaths.size();
    }
    
    if (stats.totalSize > 0) {
        stats.wastedPercentage = (double)stats.wastedSpace / stats.totalSize * 100.0;
    } else {
        stats.wastedPercentage = 0.0;
    }
    
    return stats;
}
