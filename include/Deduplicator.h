#ifndef DEDUPLICATOR_H
#define DEDUPLICATOR_H

#include "FileEntry.h"
#include <string>
#include <vector>
#include <map>
#include <set>

struct DuplicateGroup {
    std::string hash;
    std::vector<std::string> filePaths;
    long long fileSize;
    long long wastedSpace;  // (count - 1) * fileSize
    
    DuplicateGroup(const std::string& h, long long size) 
        : hash(h), fileSize(size), wastedSpace(0) {}
    
    void calculateWastedSpace() {
        wastedSpace = (filePaths.size() - 1) * fileSize;
    }
    
    std::string toString() const;
};

class Deduplicator {
private:
    std::string repositoryPath;
    std::map<std::string, DuplicateGroup> duplicateGroups;  // hash -> group
    long long totalWastedSpace;
    
    // Build hash map
    void buildHashMap(const std::vector<FileEntry>& files);
    
public:
    // Constructors
    Deduplicator();
    Deduplicator(const std::string& repoPath);
    
    // Scan for duplicates
    void scanForDuplicates();
    void scanDirectory(const std::string& dirPath);
    
    // Get results
    std::vector<DuplicateGroup> getDuplicateGroups() const;
    int getDuplicateFileCount() const;
    long long getTotalWastedSpace() const;
    
    // Display
    void printDuplicateSummary() const;
    void printDuplicateDetails() const;
    
    // Actions
    bool removeDuplicates(bool keepFirst = true);  // Keep one, delete others
    bool createHardLinks(const std::string& hash);  // Replace duplicates with hard links
    
    // Export results
    void exportToFile(const std::string& outputPath) const;
    
    // Statistics
    struct Stats {
        int totalFiles;
        int duplicateFiles;
        int uniqueFiles;
        long long totalSize;
        long long wastedSpace;
        double wastedPercentage;
    };
    
    Stats getStatistics() const;
};

#endif
