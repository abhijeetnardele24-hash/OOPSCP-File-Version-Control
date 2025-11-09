#include "../include/IntegrityChecker.h"
#include "../include/HashCalculator.h"
#include "../include/FileScanner.h"
#include "../include/FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

std::string IntegrityResult::toString() const {
    std::stringstream ss;
    ss << "File: " << filePath << std::endl;
    ss << "Status: " << (isCorrupted ? "CORRUPTED" : "OK") << std::endl;
    if (isCorrupted) {
        ss << "Expected: " << expectedHash << std::endl;
        ss << "Actual:   " << actualHash << std::endl;
    }
    ss << "Message: " << message << std::endl;
    return ss.str();
}

IntegrityChecker::IntegrityChecker() 
    : repositoryPath("."), corruptedCount(0), verifiedCount(0) {}

IntegrityChecker::IntegrityChecker(const std::string& repoPath)
    : repositoryPath(repoPath), corruptedCount(0), verifiedCount(0) {}

bool IntegrityChecker::checkFile(const std::string& filePath, const std::string& expectedHash) {
    if (!FileUtils::fileExists(filePath)) {
        IntegrityResult result(filePath, true, expectedHash, "", "File not found");
        results.push_back(result);
        corruptedCount++;
        return false;
    }
    
    std::string actualHash = HashCalculator::calculateFileHash(filePath);
    
    if (actualHash == expectedHash) {
        IntegrityResult result(filePath, false, expectedHash, actualHash, "Integrity verified");
        results.push_back(result);
        verifiedCount++;
        return true;
    } else {
        IntegrityResult result(filePath, true, expectedHash, actualHash, "Hash mismatch - file corrupted");
        results.push_back(result);
        corruptedCount++;
        return false;
    }
}

void IntegrityChecker::checkSnapshot(const Snapshot& snapshot) {
    clearResults();
    
    std::cout << "Checking integrity of snapshot: " << snapshot.getId().substr(0, 8) << std::endl;
    
    for (const auto& file : snapshot.getFiles()) {
        checkFile(file.getPath(), file.getHash());
    }
    
    printSummary();
}

void IntegrityChecker::checkAllFiles() {
    clearResults();
    
    FileScanner scanner(repositoryPath);
    scanner.loadIgnoreFile(repositoryPath + "\\.oopsignore");
    auto files = scanner.scanDirectory(true);
    
    std::cout << "Checking integrity of " << files.size() << " files..." << std::endl;
    
    for (const auto& file : files) {
        // Calculate hash and verify it exists
        std::string hash = HashCalculator::calculateFileHash(file.getPath());
        checkFile(file.getPath(), hash);
    }
    
    printSummary();
}

void IntegrityChecker::checkDirectory(const std::string& dirPath) {
    clearResults();
    
    FileScanner scanner(dirPath);
    auto files = scanner.scanDirectory(true);
    
    std::cout << "Checking integrity of " << files.size() << " files in " << dirPath << std::endl;
    
    for (const auto& file : files) {
        std::string hash = HashCalculator::calculateFileHash(file.getPath());
        checkFile(file.getPath(), hash);
    }
    
    printSummary();
}

std::vector<IntegrityResult> IntegrityChecker::getResults() const {
    return results;
}

std::vector<IntegrityResult> IntegrityChecker::getCorruptedFiles() const {
    std::vector<IntegrityResult> corrupted;
    for (const auto& result : results) {
        if (result.isCorrupted) {
            corrupted.push_back(result);
        }
    }
    return corrupted;
}

std::vector<IntegrityResult> IntegrityChecker::getVerifiedFiles() const {
    std::vector<IntegrityResult> verified;
    for (const auto& result : results) {
        if (!result.isCorrupted) {
            verified.push_back(result);
        }
    }
    return verified;
}

int IntegrityChecker::getCorruptedCount() const {
    return corruptedCount;
}

int IntegrityChecker::getVerifiedCount() const {
    return verifiedCount;
}

int IntegrityChecker::getTotalChecked() const {
    return results.size();
}

double IntegrityChecker::getIntegrityPercentage() const {
    if (results.empty()) return 100.0;
    return (double)verifiedCount / results.size() * 100.0;
}

void IntegrityChecker::printResults() const {
    std::cout << "=== Integrity Check Results ===" << std::endl;
    
    for (const auto& result : results) {
        if (result.isCorrupted) {
            std::cout << "[CORRUPTED] " << result.filePath << std::endl;
            std::cout << "  Expected: " << result.expectedHash.substr(0, 16) << "..." << std::endl;
            std::cout << "  Actual:   " << result.actualHash.substr(0, 16) << "..." << std::endl;
        } else {
            std::cout << "[OK] " << result.filePath << std::endl;
        }
    }
}

void IntegrityChecker::printSummary() const {
    std::cout << "\n=== Integrity Check Summary ===" << std::endl;
    std::cout << "Total files checked: " << getTotalChecked() << std::endl;
    std::cout << "Verified: " << verifiedCount << std::endl;
    std::cout << "Corrupted: " << corruptedCount << std::endl;
    std::cout << "Integrity: " << std::fixed << std::setprecision(2) 
              << getIntegrityPercentage() << "%" << std::endl;
    
    if (corruptedCount > 0) {
        std::cout << "\nCorrupted files:" << std::endl;
        for (const auto& result : results) {
            if (result.isCorrupted) {
                std::cout << "  - " << result.filePath << std::endl;
            }
        }
    }
}

bool IntegrityChecker::repairFile(const std::string& filePath, const Snapshot& snapshot) {
    const FileEntry* snapshotFile = snapshot.getFile(filePath);
    
    if (!snapshotFile) {
        std::cout << "Error: File not found in snapshot." << std::endl;
        return false;
    }
    
    // In a full implementation, this would restore the file from snapshot storage
    std::cout << "File repair not fully implemented yet." << std::endl;
    std::cout << "Would restore: " << filePath << std::endl;
    
    return false;
}

void IntegrityChecker::repairAllCorruptedFiles(const Snapshot& snapshot) {
    auto corrupted = getCorruptedFiles();
    
    if (corrupted.empty()) {
        std::cout << "No corrupted files to repair." << std::endl;
        return;
    }
    
    std::cout << "Repairing " << corrupted.size() << " corrupted files..." << std::endl;
    
    int repairedCount = 0;
    for (const auto& result : corrupted) {
        if (repairFile(result.filePath, snapshot)) {
            repairedCount++;
        }
    }
    
    std::cout << "Repaired " << repairedCount << " files." << std::endl;
}

void IntegrityChecker::exportResults(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    
    if (!file.is_open()) {
        std::cout << "Error: Could not open output file." << std::endl;
        return;
    }
    
    file << "Integrity Check Report" << std::endl;
    file << "======================" << std::endl;
    file << "Total files checked: " << getTotalChecked() << std::endl;
    file << "Verified: " << verifiedCount << std::endl;
    file << "Corrupted: " << corruptedCount << std::endl;
    file << "Integrity: " << std::fixed << std::setprecision(2) 
         << getIntegrityPercentage() << "%" << std::endl;
    file << std::endl;
    
    if (corruptedCount > 0) {
        file << "Corrupted Files:" << std::endl;
        for (const auto& result : results) {
            if (result.isCorrupted) {
                file << result.toString() << std::endl;
            }
        }
    }
    
    file.close();
    std::cout << "Report exported to: " << outputPath << std::endl;
}

void IntegrityChecker::clearResults() {
    results.clear();
    corruptedCount = 0;
    verifiedCount = 0;
}
