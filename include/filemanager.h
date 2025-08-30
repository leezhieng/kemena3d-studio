#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <kemena/kwindow.h>
#include <portable-file-dialogs.h>

#include "md5.h"

#ifdef _WIN32
#include <windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

using namespace kemena;
namespace fs = std::filesystem;

struct FileItem
{
    std::string name;
    bool isFile;
    std::string extension;
    // icon
};

class FileManager
{
public:
    FileManager(kWindow* setWindow);
    virtual ~FileManager();

    std::string getCurrentDirPath();
    void checkAssetsChange(const std::string& path, bool recursive = true);
    std::string fileChecksum(const std::string& fileName);
    std::string getRandomString(int stringLength);

    bool newProject();
    bool openProject();

    void refreshWindowTitle();

    // Editor path and directory
    std::string exePath;
    std::string baseDir;

    // Project info
    std::string projectName;
    bool projectOpened = false;
    bool projectSaved = true;

    // Project path and directory
    std::string projectPath;
    std::vector<std::string> currentDir;

    // World info
    string worldName = "";

private:
    kWindow* window;
    std::string initialWindowTitle;

    int initialResizeCount = 0;

    // Check project files
    std::map<std::string, int> fileGUID;
    std::map<int, std::string> fileMD5;
    std::map<int, std::string> fileCache;
    int latestFileGUID = 0;
};

#endif // FILEMANAGER_H
