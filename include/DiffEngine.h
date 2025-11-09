#ifndef DIFFENGINE_H
#define DIFFENGINE_H

#include "FileEntry.h"
#include <string>
#include <vector>

struct DiffLine {
    enum Type { UNCHANGED, ADDED, DELETED, MODIFIED };
    Type type;
    int lineNumber;
    std::string content;
    
    DiffLine(Type t, int line, const std::string& text) 
        : type(t), lineNumber(line), content(text) {}
};

class DiffEngine {
private:
    // LCS (Longest Common Subsequence) for diff algorithm
    static std::vector<std::vector<int>> computeLCS(
        const std::vector<std::string>& lines1,
        const std::vector<std::string>& lines2
    );
    
    // Generate diff from LCS
    static std::vector<DiffLine> generateDiffFromLCS(
        const std::vector<std::string>& lines1,
        const std::vector<std::string>& lines2,
        const std::vector<std::vector<int>>& lcs
    );
    
    // Read file into lines
    static std::vector<std::string> readLines(const std::string& filePath);
    
public:
    // Compare two text files and generate diff
    static std::vector<DiffLine> compareFiles(
        const std::string& file1Path,
        const std::string& file2Path
    );
    
    // Compare file with snapshot version
    static std::vector<DiffLine> compareWithSnapshot(
        const std::string& currentFile,
        const FileEntry& snapshotEntry,
        const std::string& snapshotDir
    );
    
    // Display diff in terminal
    static void printDiff(const std::vector<DiffLine>& diff);
    static void printDiffColorized(const std::vector<DiffLine>& diff);
    
    // Statistics
    static int countAdditions(const std::vector<DiffLine>& diff);
    static int countDeletions(const std::vector<DiffLine>& diff);
    static int countModifications(const std::vector<DiffLine>& diff);
    
    // Binary file comparison
    static bool areBinaryFilesIdentical(const std::string& file1, const std::string& file2);
    static std::string getBinaryFileDiffSummary(const std::string& file1, const std::string& file2);
};

#endif
