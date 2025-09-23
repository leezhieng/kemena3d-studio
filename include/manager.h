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
#include "panel_console.h"

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

// For project panel
struct FileInfo
{
    std::string path;   // path/name.ext
    std::string checksum;
    std::string type;   // model, image, prefab, etc.
};

// For converting meshes or images
struct ImportTask
{
    fs::path inputPath;
    fs::path outputPath;
    std::string type;   // model, image, etc.
    bool success = false;
    bool reported = false;
};

// For hierarchy panel
struct ObjectInfo
{
    kObject* object;   // Pointer to the object
};

class PanelProject;
class PanelHierarchy;
class PanelConsole;

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

    void checkAssetChange();

    void refreshWindowTitle();
    void closeEditor();

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
    //string worldUuid = "";

    PanelProject* panelProject;
    PanelHierarchy* panelHierarchy;

    std::vector<ImportTask> importQueue;
    std::future<void> importFuture;
    std::atomic<int> filesProcessed{0};
    std::atomic<bool> batchDone{false};
    std::mutex queueMutex;

    // Prevents asset checks from running while a message box is open, even if the application remains focused
    bool showingMessageBox = false;

    // For batch import
    bool showImportPopup = false;
    std::chrono::steady_clock::time_point importEndTime;

    std::vector<ImportTask> importTasks;
    void drawImportPopup(PanelConsole* console);

    // Check project files
    std::unordered_map<std::string, FileInfo> fileMap; // Key = uuid
    std::unordered_map<std::string, std::string> uuidMap; // Reverse lookup, key = filename

    // Check world objects
     std::unordered_map<std::string, ObjectInfo> objectMap; // Key = uuid

     std::vector<std::string> selectedObjects;
     void selectObject(const std::string uuid, bool clearList = false);
     void deselectObject(const std::string uuid);

     kObject* selectedObject = nullptr; // Temp

private:
    kWindow* window;
    kWorld* world;
    std::string initialWindowTitle;

    //int initialResizeCount = 0;

    //std::map<std::string, std::string> fileDirty;   // Files that need to be put into fileGUID, or refresh checksum into fileMD5, or regenerate thumbnail etc.
    //std::string latestFileUuid = "";

    std::string checkAssetType(const fs::path &p);
    void startBatchImport(const std::vector<ImportTask>& tasks);
};

#endif // FILEMANAGER_H
