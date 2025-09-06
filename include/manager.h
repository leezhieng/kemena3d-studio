#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <sstream>
#include <iomanip>

#include <kemena/kwindow.h>
#include <kemena/kworld.h>
#include <portable-file-dialogs.h>

#include "md5.h"

#include "panel_project.h"
#include "panel_hierarchy.h"

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

class PanelProject;
class PanelHierarchy;

class Manager
{
public:
    Manager(kWindow* setWindow, kWorld* setWorld);
    virtual ~Manager();

    std::string getCurrentDirPath();
    void checkAssetsChange(const std::string& path, bool recursive = true);
    std::string fileChecksum(const std::string& fileName);
    std::string getRandomString(int stringLength);
    std::string generateGuid();

    void openFolder(string name);
    void closeFolder();

    bool newProject();
    bool openProject();

    void refreshWindowTitle();

    // Editor path and directory
    fs::path exePath;
    fs::path baseDir;

    // Project info
    std::string projectName;
    bool projectOpened = false;
    bool projectSaved = true;

    // Project path and directory
    fs::path projectPath;
    std::vector<std::string> currentDir;

    // World info
    string worldName = "";
    string worldGUID = "";

    PanelProject* panelProject;
    PanelHierarchy* panelHierarchy;

private:
    kWindow* window;
    kWorld* world;
    std::string initialWindowTitle;

    int initialResizeCount = 0;

    // Check project files
    std::map<std::string, int> fileGUID;
    std::map<int, std::string> fileMD5;
    std::map<int, std::string> fileCache;
    int latestFileGUID = 0;
};

#endif // FILEMANAGER_H
