#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include "Repository.h"
#include "FileEntry.h"
#include <string>
#include <vector>
#include <map>

struct BackupInfo {
    std::string id;
    std::string name;
    std::string destination;
    std::time_t timestamp;
    long long totalSize;
    int fileCount;
    bool isIncremental;
    std::string baseBackupId;  // For incremental backups
    
    std::string toString() const;
};

class BackupManager {
private:
    std::string repositoryPath;
    std::string backupMetaDir;
    std::map<std::string, BackupInfo> backups;
    
    // Helper functions
    void loadBackupHistory();
    void saveBackupHistory();
    std::string generateBackupId() const;
    bool copyFilesToDestination(const std::vector<FileEntry>& files, const std::string& dest);
    
public:
    // Constructors
    BackupManager();
    BackupManager(const std::string& repoPath);
    
    // Full backup
    bool createFullBackup(const std::string& destination, const std::string& name = "");
    
    // Incremental backup
    bool createIncrementalBackup(const std::string& destination, const std::string& baseBackupId);
    
    // Restore operations
    bool restoreBackup(const std::string& backupId, const std::string& restorePath);
    bool restoreFile(const std::string& backupId, const std::string& filePath, const std::string& restorePath);
    
    // List and info
    void listBackups() const;
    BackupInfo* getBackupInfo(const std::string& backupId);
    void showBackupDetails(const std::string& backupId) const;
    
    // Deletion
    bool deleteBackup(const std::string& backupId);
    
    // Verification
    bool verifyBackup(const std::string& backupId);
    
    // Statistics
    long long getTotalBackupSize() const;
    int getBackupCount() const;
};

#endif
