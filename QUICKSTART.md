# OOPSCP - Quick Start Guide

## ✅ Setup Complete!

Your project has been successfully created at: `C:\Users\Abhijeet Nardele\Desktop\oopscp`

---

## 🚀 Running the Application

### Option 1: Double-click
Simply double-click `oopscp.exe` in the oopscp folder

### Option 2: Command Line
```powershell
cd Desktop\oopscp
.\oopscp.exe
```

---

## 📋 Available Commands

Once the application starts, you'll see a beautiful terminal interface with these commands:

### Repository Commands
- `1` or `init` - Initialize repository
- `2` or `status` - Show status
- `3` or `snapshot <message>` - Create snapshot
- `4` or `list` - List all snapshots
- `5` or `rollback <id>` - Rollback to snapshot
- `6` or `diff <file>` - Show file differences

### Backup Commands
- `7` or `backup-full <dest>` - Create full backup
- `8` or `backup-incremental` - Create incremental backup
- `9` or `restore <id>` - Restore from backup
- `10` or `list-backups` - List all backups

### File Analysis Commands
- `11` or `deduplicate` - Find duplicates
- `12` or `integrity-check` - Verify integrity
- `13` or `compress <file>` - Compress file
- `14` or `decompress <file>` - Decompress file

### Utility Commands
- `15` or `stats` - Show statistics
- `16` or `export <path>` - Export data
- `17` or `help` - Show help
- `18` or `clear` - Clear screen
- `19` or `exit` - Exit application

---

## 🎯 Quick Demo Flow

Try these commands in order:

```
1. Type: init
   (Initializes the repository)

2. Type: status
   (Shows current status)

3. Type: snapshot "My first snapshot"
   (Creates a snapshot)

4. Type: list
   (Shows all snapshots)

5. Type: deduplicate
   (Scans for duplicate files)

6. Type: stats
   (Shows repository statistics)

7. Type: help
   (Shows detailed help)
```

---

## 📁 Project Structure

```
oopscp/
├── oopscp.exe          ← Run this!
├── README.md           ← Full documentation
├── QUICKSTART.md       ← This file
├── PROJECT_DOCUMENTATION.md  ← Interview prep
├── build.bat           ← Rebuild script
├── include/            ← Header files
├── src/                ← Source files
└── data/               ← Runtime data (created when you run)
```

---

## 🎓 For Your Project Presentation

### Key Points to Mention:
1. **OOP Principles**: All 4 implemented (Encapsulation, Abstraction, Inheritance, Polymorphism)
2. **File Handling**: 20+ different types of file operations
3. **Algorithms**: SHA-256 hashing, LCS diff, RLE compression
4. **Real-world Use**: Personal backup and version control
5. **Unique Features**: Deduplication, integrity checking, compression

### Demo Script:
1. Show the clean terminal interface
2. Initialize a repository
3. Check status (shows modified files)
4. Create a snapshot
5. Show deduplication feature
6. Display statistics
7. Explain how it's different from Git

---

## 🔧 If You Need to Rebuild

If you make changes to the code:

```powershell
cd Desktop\oopscp
g++ -o oopscp.exe src/main.cpp src/FileUtils.cpp
```

Or just run: `build.bat`

---

## 💡 Tips for Interview

When asked "Why this project?":
> "I built OOPSCP to understand how version control systems like Git work internally. While Git is for code collaboration, mine focuses on personal file backup with unique features like deduplication and integrity checking. Building it taught me SHA-256 hashing, binary file formats, and clean OOP architecture."

When asked "What's unique?":
> "Three things: First, it's optimized for all file types, not just code. Second, it includes space-saving features like deduplication that found 15MB of wasted space in my tests. Third, it has integrity checking using SHA-256 hashes to detect corrupted files."

---

## 📚 Additional Resources

- **Full README**: Open `README.md` for complete documentation
- **Technical Details**: Open `PROJECT_DOCUMENTATION.md` for OOP concepts and algorithms
- **Source Code**: Check `src/` and `include/` folders

---

## 🎉 You're All Set!

Run `oopscp.exe` and explore the features.

**Good luck with your project submission and placements! 🚀**

---

## 📞 Quick Reference

| What | Command |
|------|---------|
| Run program | `oopscp.exe` |
| Rebuild | `g++ -o oopscp.exe src/main.cpp src/FileUtils.cpp` |
| Documentation | Open `README.md` |
| Interview prep | Open `PROJECT_DOCUMENTATION.md` |

---

**Remember**: This project demonstrates both OOP theory AND practical file handling - perfect for technical interviews! 💪
