#include "../include/FileUtils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <windows.h>

bool FileUtils::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && !(buffer.st_mode & S_IFDIR);
}

bool FileUtils::directoryExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && (buffer.st_mode & S_IFDIR);
}

bool FileUtils::createDirectory(const std::string& path) {
    return _mkdir(path.c_str()) == 0;
}

bool FileUtils::createDirectories(const std::string& path) {
    return _mkdir(path.c_str()) == 0;
}

long long FileUtils::getFileSize(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return buffer.st_size;
    }
    return 0;
}

std::time_t FileUtils::getLastModifiedTime(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0) {
        return buffer.st_mtime;
    }
    return 0;
}

bool FileUtils::copyFile(const std::string& source, const std::string& dest) {
    return CopyFileA(source.c_str(), dest.c_str(), FALSE) != 0;
}

bool FileUtils::deleteFile(const std::string& path) {
    return remove(path.c_str()) == 0;
}

std::string FileUtils::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileUtils::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::vector<char> FileUtils::readBinaryFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return std::vector<char>();
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    }
    
    return std::vector<char>();
}

bool FileUtils::writeBinaryFile(const std::string& path, const std::vector<char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(data.data(), data.size());
    return true;
}

std::string FileUtils::getFileExtension(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        return path.substr(dotPos);
    }
    return "";
}

std::string FileUtils::getFileName(const std::string& path) {
    size_t slashPos = path.find_last_of("\\/");
    if (slashPos != std::string::npos) {
        return path.substr(slashPos + 1);
    }
    return path;
}

std::string FileUtils::formatFileSize(long long bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024 && unitIndex < 4) {
        size /= 1024;
        unitIndex++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return oss.str();
}

std::string FileUtils::formatTimestamp(std::time_t timestamp) {
    std::tm* timeinfo = std::localtime(&timestamp);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}
