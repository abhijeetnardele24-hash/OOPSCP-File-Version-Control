#ifndef INTEGRITYCHECKER_H
#define INTEGRITYCHECKER_H

#include "FileEntry.h"
#include "Snapshot.h"
#include <string>
#include <vector>

struct IntegrityResult {
    std::string filePath;
    bool isCorrupted;
    std::string expectedHash;
    std::string actualHash;
    std::string message;
    
    IntegrityResult(const std::string& path, bool corrupted, 
                   const std::string& expected, const std::string& actual, 
                   const std::string& msg)
        : filePath(path), isCorrupted(corrupted), 
          expectedHash(expected), actualHash(actual), message(msg) {}
    
    std::string toString() const;
};

class IntegrityChecker {
private:
    std::string repositoryPath;
    std::vector<IntegrityResult> results;
    int corruptedCount;
    int verifiedCount;
    
public:
    // Constructors
    IntegrityChecker();
    IntegrityChecker(const std::string& repoPath);
    
    // Check operations
    bool checkFile(const std::string& filePath, const std::string& expectedHash);
    void checkSnapshot(const Snapshot& snapshot);
    void checkAllFiles();
    void checkDirectory(const std::string& dirPath);
    
    // Get results
    std::vector<IntegrityResult> getResults() const;
    std::vector<IntegrityResult> getCorruptedFiles() const;
    std::vector<IntegrityResult> getVerifiedFiles() const;
    
    // Statistics
    int getCorruptedCount() const;
    int getVerifiedCount() const;
    int getTotalChecked() const;
    double getIntegrityPercentage() const;
    
    // Display
    void printResults() const;
    void printSummary() const;
    
    // Repair
    bool repairFile(const std::string& filePath, const Snapshot& snapshot);
    void repairAllCorruptedFiles(const Snapshot& snapshot);
    
    // Export
    void exportResults(const std::string& outputPath) const;
    
    // Clear results
    void clearResults();
};

#endif
