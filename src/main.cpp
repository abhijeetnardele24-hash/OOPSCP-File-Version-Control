#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <windows.h>
#include "../include/Repository.h"
#include "../include/Deduplicator.h"
#include "../include/IntegrityChecker.h"
#include "../include/DiffEngine.h"
#include "../include/FileUtils.h"
#include "../include/FileScanner.h"
#include "../include/HashCalculator.h"

// Color codes for Windows terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

using namespace std;

class OopsCPCLI {
private:
    Repository* repository;
    string currentPath;
    
    void printHeader() {
        cout << CYAN << BOLD;
        cout << "=============================================================\n";
        cout << "          OOPSCP - File Sync & Version Control            \n";
        cout << "        Enterprise-Level Terminal-Based System             \n";
        cout << "=============================================================\n";
        cout << RESET << endl;
    }
    
    void printMenu() {
        cout << BOLD << "\n=== Main Menu ===\n" << RESET;
        cout << GREEN << "Repository Commands:\n" << RESET;
        cout << "  1.  init              - Initialize repository in current directory\n";
        cout << "  2.  status            - Show repository status\n";
        cout << "  3.  snapshot <msg>    - Create snapshot of current state\n";
        cout << "  4.  list              - List all snapshots\n";
        cout << "  5.  rollback <id>     - Rollback to snapshot\n";
        cout << "  6.  diff <file>       - Show differences in file\n";
        
        cout << YELLOW << "\nBackup Commands:\n" << RESET;
        cout << "  7.  backup-full <dest>      - Create full backup\n";
        cout << "  8.  backup-incremental      - Create incremental backup\n";
        cout << "  9.  restore <backup-id>     - Restore from backup\n";
        cout << "  10. list-backups            - List all backups\n";
        
        cout << MAGENTA << "\nFile Analysis Commands:\n" << RESET;
        cout << "  11. deduplicate       - Find duplicate files\n";
        cout << "  12. integrity-check   - Verify file integrity\n";
        cout << "  13. compare <file1> <file2> - Compare two files\n";
        cout << "  14. hash <file>       - Calculate SHA-256 hash\n";
        
        cout << BLUE << "\nUtility Commands:\n" << RESET;
        cout << "  15. stats             - Show repository statistics\n";
        cout << "  16. export <id> <path> - Export snapshot data\n";
        cout << "  17. help              - Show detailed help\n";
        cout << "  18. clear             - Clear screen\n";
        cout << "  19. exit              - Exit application\n";
        
        cout << "\n" << CYAN << "Current Directory: " << RESET << currentPath << endl;
        if (repository && repository->isInitialized()) {
            cout << GREEN << "[OK] Repository Initialized\n" << RESET;
        } else {
            cout << YELLOW << "[!] No repository found (use 'init')\n" << RESET;
        }
    }
    
    void handleInit() {
        cout << YELLOW << "\n[*] Initializing repository...\n" << RESET;
        
        if (repository && repository->isInitialized()) {
            cout << RED << "[ERROR] Repository already initialized.\n" << RESET;
            return;
        }
        
        delete repository;
        repository = new Repository(currentPath);
        
        if (repository->init()) {
            cout << GREEN << "[OK] Repository initialized successfully!\n" << RESET;
            cout << "Created .oopscp directory structure\n";
        } else {
            cout << RED << "[ERROR] Failed to initialize repository.\n" << RESET;
        }
    }
    
    void handleStatus() {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        repository->showStatus();
    }
    
    void handleSnapshot(const string& message) {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        cout << YELLOW << "\n[*] Creating snapshot...\n" << RESET;
        cout << "Scanning files and calculating hashes...\n";
        
        if (repository->createSnapshot(message)) {
            cout << GREEN << "[OK] Snapshot created successfully!\n" << RESET;
        } else {
            cout << RED << "[ERROR] Failed to create snapshot.\n" << RESET;
        }
    }
    
    void handleListSnapshots() {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        repository->listSnapshots();
    }
    
    void handleRollback(const string& snapshotId) {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        cout << YELLOW << "\n[*] Rolling back to snapshot " << snapshotId << "...\n" << RESET;
        if (repository->rollbackToSnapshot(snapshotId)) {
            cout << GREEN << "[OK] Rollback completed successfully!\n" << RESET;
        } else {
            cout << RED << "[ERROR] Rollback failed.\n" << RESET;
        }
    }
    
    void handleDiff(const string& filename) {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        if (!FileUtils::fileExists(filename)) {
            cout << RED << "[ERROR] File not found: " << filename << "\n" << RESET;
            return;
        }
        
        cout << BOLD << "\n[*] Diff for " << filename << ":\n" << RESET;
        cout << "=======================================\n";
        
        Snapshot* latest = repository->getLatestSnapshot();
        if (!latest) {
            cout << YELLOW << "[!] No snapshots found. Create a snapshot first.\n" << RESET;
            return;
        }
        
        const FileEntry* snapshotFile = latest->getFile(filename);
        if (!snapshotFile) {
            cout << YELLOW << "[!] File not in latest snapshot (new file?)\n" << RESET;
            return;
        }
        
        // Calculate current hash
        string currentHash = HashCalculator::calculateFileHash(filename);
        if (currentHash == snapshotFile->getHash()) {
            cout << GREEN << "No changes detected.\n" << RESET;
        } else {
            cout << YELLOW << "File has been modified.\n" << RESET;
            cout << "Expected hash: " << snapshotFile->getHash().substr(0, 16) << "...\n";
            cout << "Current hash:  " << currentHash.substr(0, 16) << "...\n";
        }
    }
    
    void handleCompareFiles(const string& file1, const string& file2) {
        if (!FileUtils::fileExists(file1)) {
            cout << RED << "[ERROR] File not found: " << file1 << "\n" << RESET;
            return;
        }
        if (!FileUtils::fileExists(file2)) {
            cout << RED << "[ERROR] File not found: " << file2 << "\n" << RESET;
            return;
        }
        
        cout << BOLD << "\n[*] Comparing files:\n" << RESET;
        cout << "File 1: " << file1 << "\n";
        cout << "File 2: " << file2 << "\n";
        cout << "=======================================\n";
        
        auto diff = DiffEngine::compareFiles(file1, file2);
        
        if (diff.empty()) {
            cout << GREEN << "Files are identical.\n" << RESET;
            return;
        }
        
        DiffEngine::printDiffColorized(diff);
        
        cout << "\n" << CYAN << "Summary:\n" << RESET;
        cout << "  Additions:    " << GREEN << DiffEngine::countAdditions(diff) << RESET << "\n";
        cout << "  Deletions:    " << RED << DiffEngine::countDeletions(diff) << RESET << "\n";
        cout << "  Modifications: " << YELLOW << DiffEngine::countModifications(diff) << RESET << "\n";
    }
    
    void handleHash(const string& filename) {
        if (!FileUtils::fileExists(filename)) {
            cout << RED << "[ERROR] File not found: " << filename << "\n" << RESET;
            return;
        }
        
        cout << YELLOW << "\n[*] Calculating SHA-256 hash...\n" << RESET;
        string hash = HashCalculator::calculateFileHash(filename);
        
        if (hash.empty()) {
            cout << RED << "[ERROR] Failed to calculate hash.\n" << RESET;
            return;
        }
        
        cout << BOLD << "File: " << RESET << filename << "\n";
        cout << BOLD << "SHA-256: " << RESET << hash << "\n";
        cout << BOLD << "Size: " << RESET << FileUtils::formatFileSize(FileUtils::getFileSize(filename)) << "\n";
    }
    
    void handleDeduplicate() {
        cout << YELLOW << "\n[*] Scanning for duplicate files...\n" << RESET;
        cout << "This may take a while for large directories...\n";
        
        Deduplicator dedup(currentPath);
        dedup.scanForDuplicates();
        
        cout << "\n";
        dedup.printDuplicateSummary();
        
        if (dedup.getDuplicateFileCount() > 0) {
            cout << "\n" << CYAN << "Show detailed list? (y/n): " << RESET;
            string response;
            getline(cin, response);
            if (response == "y" || response == "Y") {
                dedup.printDuplicateDetails();
            }
        }
    }
    
    void handleIntegrityCheck() {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        cout << YELLOW << "\n[*] Performing integrity check...\n" << RESET;
        
        Snapshot* latest = repository->getLatestSnapshot();
        if (!latest) {
            cout << YELLOW << "[!] No snapshots found. Create a snapshot first.\n" << RESET;
            return;
        }
        
        IntegrityChecker checker(currentPath);
        checker.checkSnapshot(*latest);
    }
    
    void handleStats() {
        cout << BOLD << "\n[*] Repository Statistics:\n" << RESET;
        cout << "=======================================\n";
        
        // Scan current directory
        cout << YELLOW << "Scanning directory...\n" << RESET;
        FileScanner scanner(currentPath);
        scanner.loadIgnoreFile(currentPath + "\\.oopsignore");
        auto files = scanner.scanDirectory(true);
        
        long long totalSize = 0;
        for (const auto& file : files) {
            totalSize += file.getSize();
        }
        
        cout << "\n" << CYAN << "General:\n" << RESET;
        cout << "  Total files: " << files.size() << "\n";
        cout << "  Total size: " << FileUtils::formatFileSize(totalSize) << "\n";
        
        if (repository && repository->isInitialized()) {
            auto snapshots = repository->getAllSnapshots();
            cout << "  Snapshots: " << snapshots.size() << "\n";
            
            if (!snapshots.empty()) {
                cout << "  Last snapshot: " << FileUtils::formatTimestamp(snapshots[0].getTimestamp()) << "\n";
            }
        }
        
        // Deduplication stats
        cout << "\n" << CYAN << "Analyzing for duplicates...\n" << RESET;
        Deduplicator dedup(currentPath);
        dedup.scanForDuplicates();
        
        cout << "\n" << CYAN << "Storage Optimization:\n" << RESET;
        cout << "  Duplicate files: " << dedup.getDuplicateFileCount() << "\n";
        cout << "  Wasted space: " << RED << FileUtils::formatFileSize(dedup.getTotalWastedSpace()) << RESET << "\n";
        
        if (totalSize > 0) {
            double wastedPercent = (double)dedup.getTotalWastedSpace() / totalSize * 100.0;
            cout << "  Wasted percentage: " << RED << fixed << setprecision(2) << wastedPercent << "%" << RESET << "\n";
        }
    }
    
    void handleExport(const string& snapshotId, const string& exportPath) {
        if (!repository || !repository->isInitialized()) {
            cout << RED << "[ERROR] Not a repository. Run 'init' first.\n" << RESET;
            return;
        }
        
        Snapshot* snapshot = repository->getSnapshot(snapshotId);
        if (!snapshot) {
            cout << RED << "[ERROR] Snapshot not found: " << snapshotId << "\n" << RESET;
            return;
        }
        
        cout << YELLOW << "Exporting snapshot to " << exportPath << "...\n" << RESET;
        
        if (snapshot->save(exportPath)) {
            cout << GREEN << "[OK] Snapshot exported successfully!\n" << RESET;
        } else {
            cout << RED << "[ERROR] Failed to export snapshot.\n" << RESET;
        }
    }
    
    void handleHelp() {
        cout << BOLD << "\n[*] OOPSCP - Detailed Help\n" << RESET;
        cout << "===============================================================\n";
        
        cout << CYAN << "\nVersion Control:\n" << RESET;
        cout << "  init                  Initialize a new repository\n";
        cout << "  snapshot <message>    Create a snapshot with description\n";
        cout << "  rollback <id>         Restore files to snapshot state\n";
        cout << "  diff <file>           Show if file changed from snapshot\n";
        cout << "  status                Show modified/new/deleted files\n";
        
        cout << CYAN << "\nFile Analysis:\n" << RESET;
        cout << "  deduplicate           Find duplicate files by hash\n";
        cout << "  integrity-check       Verify file integrity (SHA-256)\n";
        cout << "  compare <f1> <f2>     Line-by-line comparison (LCS algorithm)\n";
        cout << "  hash <file>           Calculate SHA-256 hash of file\n";
        
        cout << CYAN << "\nFeatures:\n" << RESET;
        cout << "  • Real SHA-256 cryptographic hashing\n";
        cout << "  • Binary file serialization for snapshots\n";
        cout << "  • LCS algorithm for text file comparison\n";
        cout << "  • Duplicate file detection with hash maps\n";
        cout << "  • File integrity verification\n";
        cout << "  • .oopsignore support (like .gitignore)\n";
        cout << "  • Real directory traversal and file scanning\n";
    }
    
public:
    OopsCPCLI() : repository(nullptr), currentPath(".") {
        // Get current working directory
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        currentPath = buffer;
        
        // Check if repository exists
        if (Repository::isRepository(currentPath)) {
            repository = new Repository(currentPath);
        }
    }
    
    ~OopsCPCLI() {
        delete repository;
    }
    
    void run() {
        printHeader();
        
        string command;
        while (true) {
            printMenu();
            cout << "\n" << BOLD << "oopscp> " << RESET;
            
            getline(cin, command);
            
            if (command.empty()) continue;
            
            // Parse command and arguments
            vector<string> tokens;
            size_t pos = 0;
            while (pos < command.length()) {
                size_t nextSpace = command.find(' ', pos);
                if (nextSpace == string::npos) {
                    tokens.push_back(command.substr(pos));
                    break;
                }
                tokens.push_back(command.substr(pos, nextSpace - pos));
                pos = nextSpace + 1;
            }
            
            if (tokens.empty()) continue;
            
            string cmd = tokens[0];
            
            // Handle commands
            if (cmd == "init" || cmd == "1") {
                handleInit();
            }
            else if (cmd == "status" || cmd == "2") {
                handleStatus();
            }
            else if (cmd == "snapshot" || cmd == "3") {
                string message = tokens.size() > 1 ? command.substr(command.find(' ') + 1) : "No message";
                handleSnapshot(message);
            }
            else if (cmd == "list" || cmd == "4") {
                handleListSnapshots();
            }
            else if (cmd == "rollback" || cmd == "5") {
                string id = tokens.size() > 1 ? tokens[1] : "";
                if (id.empty()) {
                    cout << RED << "[ERROR] Usage: rollback <snapshot-id>\n" << RESET;
                } else {
                    handleRollback(id);
                }
            }
            else if (cmd == "diff" || cmd == "6") {
                string file = tokens.size() > 1 ? tokens[1] : "";
                if (file.empty()) {
                    cout << RED << "[ERROR] Usage: diff <filename>\n" << RESET;
                } else {
                    handleDiff(file);
                }
            }
            else if (cmd == "backup-full" || cmd == "7") {
                cout << YELLOW << "Full backup feature - under development\n" << RESET;
            }
            else if (cmd == "backup-incremental" || cmd == "8") {
                cout << YELLOW << "Incremental backup feature - under development\n" << RESET;
            }
            else if (cmd == "restore" || cmd == "9") {
                cout << YELLOW << "Restore feature - under development\n" << RESET;
            }
            else if (cmd == "list-backups" || cmd == "10") {
                cout << YELLOW << "Backup listing feature - under development\n" << RESET;
            }
            else if (cmd == "deduplicate" || cmd == "11") {
                handleDeduplicate();
            }
            else if (cmd == "integrity-check" || cmd == "12") {
                handleIntegrityCheck();
            }
            else if (cmd == "compare" || cmd == "13") {
                if (tokens.size() < 3) {
                    cout << RED << "[ERROR] Usage: compare <file1> <file2>\n" << RESET;
                } else {
                    handleCompareFiles(tokens[1], tokens[2]);
                }
            }
            else if (cmd == "hash" || cmd == "14") {
                if (tokens.size() < 2) {
                    cout << RED << "[ERROR] Usage: hash <filename>\n" << RESET;
                } else {
                    handleHash(tokens[1]);
                }
            }
            else if (cmd == "stats" || cmd == "15") {
                handleStats();
            }
            else if (cmd == "export" || cmd == "16") {
                if (tokens.size() < 3) {
                    cout << RED << "[ERROR] Usage: export <snapshot-id> <output-path>\n" << RESET;
                } else {
                    handleExport(tokens[1], tokens[2]);
                }
            }
            else if (cmd == "help" || cmd == "17") {
                handleHelp();
            }
            else if (cmd == "clear" || cmd == "18") {
                system("cls");  // Windows
                printHeader();
            }
            else if (cmd == "exit" || cmd == "19") {
                cout << CYAN << "\nThank you for using OOPSCP!\n" << RESET;
                break;
            }
            else {
                cout << RED << "[ERROR] Unknown command: " << cmd << "\n" << RESET;
                cout << "Type 'help' for available commands.\n";
            }
            
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
};

int main() {
    // Enable ANSI colors in Windows terminal
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    OopsCPCLI cli;
    cli.run();
    
    return 0;
}
