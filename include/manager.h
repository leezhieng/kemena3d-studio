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
    //void checkAssetsChange(const std::string& path, bool recursive = true);

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
    string worldUuid = "";

    PanelProject* panelProject;
    PanelHierarchy* panelHierarchy;

private:
    kWindow* window;
    kWorld* world;
    std::string initialWindowTitle;

    int initialResizeCount = 0;

    // Check project files
    std::map<std::string, std::string> fileUuid;        // File GUID and its file name (path/filename.ext)
    std::map<std::string, std::string> fileChecksum;    // File GUID and its checksum in MD5 format
    std::map<std::string, int> fileType;                // File GUID and its type (0 - mesh, 1 - material, etc)

    std::map<std::string, std::string> fileDirty;   // Files that need to be put into fileGUID, or refresh checksum into fileMD5, or regenerate thumbnail etc.
    std::string latestFileUuid = "";

    void checkDirJson();
    int checkAssetType(const fs::path &p);
};

#endif // FILEMANAGER_H
