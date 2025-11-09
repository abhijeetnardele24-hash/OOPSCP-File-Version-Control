#include "../include/DiffEngine.h"
#include "../include/FileUtils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>

std::vector<std::string> DiffEngine::readLines(const std::string& filePath) {
    std::vector<std::string> lines;
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        return lines;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    file.close();
    return lines;
}

std::vector<std::vector<int>> DiffEngine::computeLCS(
    const std::vector<std::string>& lines1,
    const std::vector<std::string>& lines2
) {
    int m = lines1.size();
    int n = lines2.size();
    
    // Create LCS table
    std::vector<std::vector<int>> lcs(m + 1, std::vector<int>(n + 1, 0));
    
    // Build LCS table using dynamic programming
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (lines1[i - 1] == lines2[j - 1]) {
                lcs[i][j] = lcs[i - 1][j - 1] + 1;
            } else {
                lcs[i][j] = std::max(lcs[i - 1][j], lcs[i][j - 1]);
            }
        }
    }
    
    return lcs;
}

std::vector<DiffLine> DiffEngine::generateDiffFromLCS(
    const std::vector<std::string>& lines1,
    const std::vector<std::string>& lines2,
    const std::vector<std::vector<int>>& lcs
) {
    std::vector<DiffLine> diff;
    int i = lines1.size();
    int j = lines2.size();
    
    // Backtrack through LCS table to generate diff
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && lines1[i - 1] == lines2[j - 1]) {
            // Lines are the same - unchanged
            diff.push_back(DiffLine(DiffLine::UNCHANGED, i, lines1[i - 1]));
            i--;
            j--;
        } else if (j > 0 && (i == 0 || lcs[i][j - 1] >= lcs[i - 1][j])) {
            // Line added in file2
            diff.push_back(DiffLine(DiffLine::ADDED, j, lines2[j - 1]));
            j--;
        } else if (i > 0 && (j == 0 || lcs[i][j - 1] < lcs[i - 1][j])) {
            // Line deleted from file1
            diff.push_back(DiffLine(DiffLine::DELETED, i, lines1[i - 1]));
            i--;
        }
    }
    
    // Reverse since we built it backwards
    std::reverse(diff.begin(), diff.end());
    
    return diff;
}

std::vector<DiffLine> DiffEngine::compareFiles(
    const std::string& file1Path,
    const std::string& file2Path
) {
    auto lines1 = readLines(file1Path);
    auto lines2 = readLines(file2Path);
    
    auto lcs = computeLCS(lines1, lines2);
    return generateDiffFromLCS(lines1, lines2, lcs);
}

std::vector<DiffLine> DiffEngine::compareWithSnapshot(
    const std::string& currentFile,
    const FileEntry& snapshotEntry,
    const std::string& snapshotDir
) {
    // In a full implementation, this would retrieve the file from snapshot storage
    // For now, we'll just compare with current file
    return std::vector<DiffLine>();
}

void DiffEngine::printDiff(const std::vector<DiffLine>& diff) {
    for (const auto& line : diff) {
        switch (line.type) {
            case DiffLine::UNCHANGED:
                std::cout << "  " << line.content << std::endl;
                break;
            case DiffLine::ADDED:
                std::cout << "+ " << line.content << std::endl;
                break;
            case DiffLine::DELETED:
                std::cout << "- " << line.content << std::endl;
                break;
            case DiffLine::MODIFIED:
                std::cout << "M " << line.content << std::endl;
                break;
        }
    }
}

void DiffEngine::printDiffColorized(const std::vector<DiffLine>& diff) {
    // ANSI color codes
    const char* GREEN = "\033[32m";
    const char* RED = "\033[31m";
    const char* RESET = "\033[0m";
    
    for (const auto& line : diff) {
        switch (line.type) {
            case DiffLine::UNCHANGED:
                std::cout << "  " << line.content << std::endl;
                break;
            case DiffLine::ADDED:
                std::cout << GREEN << "+ " << line.content << RESET << std::endl;
                break;
            case DiffLine::DELETED:
                std::cout << RED << "- " << line.content << RESET << std::endl;
                break;
            case DiffLine::MODIFIED:
                std::cout << "M " << line.content << std::endl;
                break;
        }
    }
}

int DiffEngine::countAdditions(const std::vector<DiffLine>& diff) {
    int count = 0;
    for (const auto& line : diff) {
        if (line.type == DiffLine::ADDED) count++;
    }
    return count;
}

int DiffEngine::countDeletions(const std::vector<DiffLine>& diff) {
    int count = 0;
    for (const auto& line : diff) {
        if (line.type == DiffLine::DELETED) count++;
    }
    return count;
}

int DiffEngine::countModifications(const std::vector<DiffLine>& diff) {
    int count = 0;
    for (const auto& line : diff) {
        if (line.type == DiffLine::MODIFIED) count++;
    }
    return count;
}

bool DiffEngine::areBinaryFilesIdentical(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);
    
    if (!f1.is_open() || !f2.is_open()) return false;
    
    // Get file sizes
    f1.seekg(0, std::ios::end);
    f2.seekg(0, std::ios::end);
    
    long long size1 = f1.tellg();
    long long size2 = f2.tellg();
    
    if (size1 != size2) return false;
    
    f1.seekg(0, std::ios::beg);
    f2.seekg(0, std::ios::beg);
    
    // Compare byte by byte
    const size_t BUFFER_SIZE = 8192;
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    
    while (!f1.eof() && !f2.eof()) {
        f1.read(buffer1, BUFFER_SIZE);
        f2.read(buffer2, BUFFER_SIZE);
        
        size_t bytes1 = f1.gcount();
        size_t bytes2 = f2.gcount();
        
        if (bytes1 != bytes2) return false;
        
        if (memcmp(buffer1, buffer2, bytes1) != 0) return false;
    }
    
    return true;
}

std::string DiffEngine::getBinaryFileDiffSummary(const std::string& file1, const std::string& file2) {
    if (areBinaryFilesIdentical(file1, file2)) {
        return "Binary files are identical";
    }
    
    long long size1 = FileUtils::getFileSize(file1);
    long long size2 = FileUtils::getFileSize(file2);
    
    std::stringstream ss;
    ss << "Binary files differ\n";
    ss << "  File 1: " << FileUtils::formatFileSize(size1) << "\n";
    ss << "  File 2: " << FileUtils::formatFileSize(size2) << "\n";
    ss << "  Difference: ";
    
    if (size2 > size1) {
        ss << "+" << FileUtils::formatFileSize(size2 - size1);
    } else {
        ss << "-" << FileUtils::formatFileSize(size1 - size2);
    }
    
    return ss.str();
}
