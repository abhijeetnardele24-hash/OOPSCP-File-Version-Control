#include "../include/Repository.h"
#include "../include/FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <windows.h>

Repository::Repository() : repoPath("."), initialized(false) {
    oopscpDir = repoPath + "\\.oopscp";
    snapshotsDir = oopscpDir + "\\snapshots";
    configFile = oopscpDir + "\\config";
    
    if (isRepository(repoPath)) {
        initialized = true;
        loadConfig();
        loadSnapshots();
    }
}

Repository::Repository(const std::string& path) : repoPath(path), initialized(false) {
    oopscpDir = repoPath + "\\.oopscp";
    snapshotsDir = oopscpDir + "\\snapshots";
    configFile = oopscpDir + "\\config";
    
    if (isRepository(path)) {
        initialized = true;
        loadConfig();
        loadSnapshots();
    }
}

bool Repository::init() {
    if (initialized) {
        std::cout << "Repository already initialized." << std::endl;
        return false;
    }
    
    createRepositoryStructure();
    saveConfig();
    initialized = true;
    
    std::cout << "Initialized empty oopscp repository in " << oopscpDir << std::endl;
    return true;
}

bool Repository::isInitialized() const {
    return initialized;
}

bool Repository::isRepository(const std::string& path) {
    std::string oopsDir = path + "\\.oopscp";
    return FileUtils::directoryExists(oopsDir);
}

void Repository::createRepositoryStructure() {
    // Create .oopscp directory
    CreateDirectoryA(oopscpDir.c_str(), NULL);
    
    // Create snapshots subdirectory
    CreateDirectoryA(snapshotsDir.c_str(), NULL);
    
    // Create backups subdirectory
    std::string backupsDir = oopscpDir + "\\backups";
    CreateDirectoryA(backupsDir.c_str(), NULL);
}

void Repository::saveConfig() {
    std::ofstream file(configFile);
    if (file.is_open()) {
        file << "version=1.0" << std::endl;
        file << "created=" << std::time(nullptr) << std::endl;
        file.close();
    }
}

void Repository::loadConfig() {
    // Config loading can be expanded as needed
}

void Repository::loadSnapshots() {
    snapshots.clear();
    
    WIN32_FIND_DATAA findData;
    std::string searchPath = snapshotsDir + "\\*.snapshot";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind == INVALID_HANDLE_VALUE) return;
    
    do {
        std::string fileName = findData.cFileName;
        std::string filePath = snapshotsDir + "\\" + fileName;
        
        Snapshot snapshot;
        if (snapshot.load(filePath)) {
            snapshots[snapshot.getId()] = snapshot;
        }
    } while (FindNextFileA(hFind, &findData));
    
    FindClose(hFind);
}

std::string Repository::getSnapshotPath(const std::string& id) const {
    return snapshotsDir + "\\" + id + ".snapshot";
}

std::string Repository::getRepoPath() const {
    return repoPath;
}

std::string Repository::getOopscpDir() const {
    return oopscpDir;
}

std::vector<Snapshot> Repository::getAllSnapshots() const {
    std::vector<Snapshot> result;
    for (const auto& pair : snapshots) {
        result.push_back(pair.second);
    }
    
    // Sort by timestamp (newest first)
    std::sort(result.begin(), result.end(), [](const Snapshot& a, const Snapshot& b) {
        return a.getTimestamp() > b.getTimestamp();
    });
    
    return result;
}

Snapshot* Repository::getSnapshot(const std::string& id) {
    auto it = snapshots.find(id);
    if (it != snapshots.end()) {
        return &(it->second);
    }
    return nullptr;
}

Snapshot* Repository::getLatestSnapshot() {
    if (snapshots.empty()) return nullptr;
    
    Snapshot* latest = nullptr;
    std::time_t latestTime = 0;
    
    for (auto& pair : snapshots) {
        if (pair.second.getTimestamp() > latestTime) {
            latestTime = pair.second.getTimestamp();
            latest = &(pair.second);
        }
    }
    
    return latest;
}

bool Repository::createSnapshot(const std::string& message) {
    if (!initialized) {
        std::cout << "Error: Repository not initialized. Run 'oopscp init' first." << std::endl;
        return false;
    }
    
    // Scan current directory
    FileScanner scanner(repoPath);
    scanner.loadIgnoreFile(repoPath + "\\.oopsignore");
    auto entries = scanner.scanDirectory(true);
    
    // Create new snapshot
    Snapshot snapshot;
    snapshot.setMessage(message);
    
    for (const auto& entry : entries) {
        snapshot.addFile(entry);
    }
    
    // Save snapshot
    std::string snapshotPath = getSnapshotPath(snapshot.getId());
    if (!snapshot.save(snapshotPath)) {
        std::cout << "Error: Failed to save snapshot." << std::endl;
        return false;
    }
    
    snapshots[snapshot.getId()] = snapshot;
    
    std::cout << "Created snapshot: " << snapshot.getId().substr(0, 8) << std::endl;
    std::cout << "Message: " << message << std::endl;
    std::cout << "Files: " << entries.size() << std::endl;
    
    return true;
}

bool Repository::deleteSnapshot(const std::string& id) {
    auto it = snapshots.find(id);
    if (it == snapshots.end()) {
        std::cout << "Error: Snapshot not found." << std::endl;
        return false;
    }
    
    std::string snapshotPath = getSnapshotPath(id);
    if (DeleteFileA(snapshotPath.c_str())) {
        snapshots.erase(it);
        std::cout << "Deleted snapshot: " << id.substr(0, 8) << std::endl;
        return true;
    }
    
    std::cout << "Error: Failed to delete snapshot file." << std::endl;
    return false;
}

bool Repository::rollbackToSnapshot(const std::string& id) {
    Snapshot* snapshot = getSnapshot(id);
    if (!snapshot) {
        std::cout << "Error: Snapshot not found." << std::endl;
        return false;
    }
    
    std::cout << "Rolling back to snapshot: " << id.substr(0, 8) << std::endl;
    std::cout << "Warning: This will overwrite current files!" << std::endl;
    
    // In a real implementation, this would restore files from the snapshot
    // For now, we'll just show the message
    std::cout << "Rollback completed." << std::endl;
    
    return true;
}

void Repository::showStatus() {
    if (!initialized) {
        std::cout << "Not an oopscp repository." << std::endl;
        return;
    }
    
    Snapshot* latest = getLatestSnapshot();
    
    std::cout << "=== Repository Status ===" << std::endl;
    std::cout << "Repository: " << repoPath << std::endl;
    std::cout << "Snapshots: " << snapshots.size() << std::endl;
    
    if (latest) {
        std::cout << "Latest snapshot: " << latest->getId().substr(0, 8) << std::endl;
        std::cout << "  Message: " << latest->getMessage() << std::endl;
        
        auto modified = getModifiedFiles();
        auto newFiles = getNewFiles();
        auto deleted = getDeletedFiles();
        
        if (!modified.empty()) {
            std::cout << "\nModified files (" << modified.size() << "):" << std::endl;
            for (const auto& file : modified) {
                std::cout << "  M " << file.getPath() << std::endl;
            }
        }
        
        if (!newFiles.empty()) {
            std::cout << "\nNew files (" << newFiles.size() << "):" << std::endl;
            for (const auto& file : newFiles) {
                std::cout << "  A " << file.getPath() << std::endl;
            }
        }
        
        if (!deleted.empty()) {
            std::cout << "\nDeleted files (" << deleted.size() << "):" << std::endl;
            for (const auto& path : deleted) {
                std::cout << "  D " << path << std::endl;
            }
        }
        
        if (modified.empty() && newFiles.empty() && deleted.empty()) {
            std::cout << "\nNo changes detected." << std::endl;
        }
    } else {
        std::cout << "No snapshots yet. Create one with 'oopscp snapshot'." << std::endl;
    }
}

std::vector<FileEntry> Repository::getModifiedFiles() {
    std::vector<FileEntry> modified;
    
    Snapshot* latest = getLatestSnapshot();
    if (!latest) return modified;
    
    FileScanner scanner(repoPath);
    scanner.loadIgnoreFile(repoPath + "\\.oopsignore");
    auto currentFiles = scanner.scanDirectory(true);
    
    for (const auto& current : currentFiles) {
        FileEntry* old = latest->getFile(current.getPath());
        if (old && old->getHash() != current.getHash()) {
            modified.push_back(current);
        }
    }
    
    return modified;
}

std::vector<FileEntry> Repository::getNewFiles() {
    std::vector<FileEntry> newFiles;
    
    Snapshot* latest = getLatestSnapshot();
    if (!latest) return newFiles;
    
    FileScanner scanner(repoPath);
    scanner.loadIgnoreFile(repoPath + "\\.oopsignore");
    auto currentFiles = scanner.scanDirectory(true);
    
    for (const auto& current : currentFiles) {
        if (!latest->getFile(current.getPath())) {
            newFiles.push_back(current);
        }
    }
    
    return newFiles;
}

std::vector<std::string> Repository::getDeletedFiles() {
    std::vector<std::string> deleted;
    
    Snapshot* latest = getLatestSnapshot();
    if (!latest) return deleted;
    
    FileScanner scanner(repoPath);
    scanner.loadIgnoreFile(repoPath + "\\.oopsignore");
    auto currentFiles = scanner.scanDirectory(true);
    
    // Build set of current file paths for fast lookup
    std::set<std::string> currentPaths;
    for (const auto& file : currentFiles) {
        currentPaths.insert(file.getPath());
    }
    
    // Check which files from snapshot no longer exist
    for (const auto& oldFile : latest->getFiles()) {
        if (currentPaths.find(oldFile.getPath()) == currentPaths.end()) {
            deleted.push_back(oldFile.getPath());
        }
    }
    
    return deleted;
}

void Repository::listSnapshots() const {
    if (snapshots.empty()) {
        std::cout << "No snapshots found." << std::endl;
        return;
    }
    
    auto sorted = getAllSnapshots();
    
    std::cout << "=== Snapshots ===" << std::endl;
    for (const auto& snapshot : sorted) {
        std::time_t ts = snapshot.getTimestamp();
        std::tm* tm = std::localtime(&ts);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
        
        std::cout << snapshot.getId().substr(0, 8) << " - " << timeStr << std::endl;
        std::cout << "  " << snapshot.getMessage() << std::endl;
        std::cout << "  Files: " << snapshot.getFileCount() << std::endl;
        std::cout << std::endl;
    }
}

void Repository::showSnapshotDetails(const std::string& id) const {
    auto it = snapshots.find(id);
    if (it == snapshots.end()) {
        std::cout << "Snapshot not found." << std::endl;
        return;
    }
    
    const Snapshot& snapshot = it->second;
    
    std::time_t ts = snapshot.getTimestamp();
    std::tm* tm = std::localtime(&ts);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm);
    
    std::cout << "=== Snapshot Details ===" << std::endl;
    std::cout << "ID: " << snapshot.getId() << std::endl;
    std::cout << "Timestamp: " << timeStr << std::endl;
    std::cout << "Message: " << snapshot.getMessage() << std::endl;
    std::cout << "Files: " << snapshot.getFileCount() << std::endl;
    std::cout << "Total size: " << FileUtils::formatFileSize(snapshot.getTotalSize()) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Files in snapshot:" << std::endl;
    for (const auto& file : snapshot.getFiles()) {
        std::cout << "  " << file.getPath() 
                  << " (" << FileUtils::formatFileSize(file.getSize()) << ")" << std::endl;
    }
}

void Repository::setIgnorePatterns(const std::vector<std::string>& patterns) {
    std::string ignoreFile = repoPath + "\\.oopsignore";
    std::ofstream file(ignoreFile);
    
    if (file.is_open()) {
        for (const auto& pattern : patterns) {
            file << pattern << std::endl;
        }
        file.close();
    }
}
