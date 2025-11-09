#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "Snapshot.h"
#include "FileEntry.h"
#include "FileScanner.h"
#include <string>
#include <vector>
#include <map>

class Repository {
private:
    std::string repoPath;              // Path to repository root
    std::string oopscpDir;             // Path to .oopscp directory (metadata)
    std::string snapshotsDir;          // Path to snapshots directory
    std::string configFile;            // Path to config file
    std::map<std::string, Snapshot> snapshots;  // All snapshots (id -> snapshot)
    bool initialized;                  // Is repository initialized?
    
    // Helper functions
    void createRepositoryStructure();
    void loadSnapshots();
    void saveConfig();
    void loadConfig();
    std::string getSnapshotPath(const std::string& id) const;
    
public:
    // Constructors
    Repository();
    Repository(const std::string& path);
    
    // Initialization
    bool init();
    bool isInitialized() const;
    static bool isRepository(const std::string& path);
    
    // Getters
    std::string getRepoPath() const;
    std::string getOopscpDir() const;
    std::vector<Snapshot> getAllSnapshots() const;
    Snapshot* getSnapshot(const std::string& id);
    Snapshot* getLatestSnapshot();
    
    // Snapshot operations
    bool createSnapshot(const std::string& message);
    bool deleteSnapshot(const std::string& id);
    bool rollbackToSnapshot(const std::string& id);
    
    // Status
    void showStatus();
    std::vector<FileEntry> getModifiedFiles();
    std::vector<FileEntry> getNewFiles();
    std::vector<std::string> getDeletedFiles();
    
    // Display
    void listSnapshots() const;
    void showSnapshotDetails(const std::string& id) const;
    
    // Configuration
    void setIgnorePatterns(const std::vector<std::string>& patterns);
};

#endif
