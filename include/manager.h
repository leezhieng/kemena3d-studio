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
using json = nlohmann::json;

struct FileInfo {
    std::string path;   // path/name.ext
    std::string checksum;
    std::string type;   // model, texture, prefab, etc.
};

class PanelProject;
class PanelHierarchy;

class Manager
{
public:
    Manager(kWindow* setWindow, kWorld* setWorld);
    virtual ~Manager();

    std::string getCurrentDirPath();

    void openFolder(string name);
    void closeFolder();

    bool newProject();
    bool openProject();

    void checkAssetJson();

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
    string worldUuid = "";

    PanelProject* panelProject;
    PanelHierarchy* panelHierarchy;

private:
    kWindow* window;
    kWorld* world;
    std::string initialWindowTitle;

    int initialResizeCount = 0;

    // Check project files
    std::unordered_map<std::string, FileInfo> fileMap; // key = uuid

    std::map<std::string, std::string> fileDirty;   // Files that need to be put into fileGUID, or refresh checksum into fileMD5, or regenerate thumbnail etc.
    std::string latestFileUuid = "";

    std::string checkAssetType(const fs::path &p);
};

#endif // FILEMANAGER_H
