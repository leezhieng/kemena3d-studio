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
#include <kemena/krenderer.h>
#include <kemena/kscene.h>
#include <kemena/kmeshgenerator.h>
#include <kemena/klight.h>
#include <kemena/kcamera.h>
#include <kemena/kassetmanager.h>
#include <kemena/koffscreenrenderer.h>

#include "commands.h"
#include <portable-file-dialogs.h>
#include <kemena/kguimanager.h>
#include <ImGuizmo.h>

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
    kString path; // path/name.ext
    kString checksum;
    kString type; // model, image, prefab, etc.
};

// For converting meshes or images
struct ImportTask
{
    fs::path inputPath;
    fs::path outputPath;
    kString  type;    // mesh, image, etc.
    kString  uuid;
    fs::path thumbnailPath; // where to save uuid.png after import
    bool success  = false;
    bool reported = false;
};

// Queued thumbnail render (processed on main thread after batch import)
struct ThumbnailTask
{
    kString  uuid;
    fs::path srcPath;       // GLB path for mesh, original image path for image
    fs::path thumbnailPath;
    kString  type;          // "mesh" or "image"
};

// For hierarchy panel
struct ObjectInfo
{
    kObject *object; // Pointer to the object
};

class PanelProject;
class PanelHierarchy;
class PanelConsole;

class Manager
{
public:
    Manager(kWindow *setWindow, kWorld *setWorld, kRenderer *setRenderer);
    virtual ~Manager();

    void setScene(kScene *s)  { scene = s; }
    kScene    *getScene()     { return scene; }
    kRenderer *getRenderer()  { return renderer; }

    kObject *findObjectByUuid(const kString &uuid);
    void deleteSelectedObjects();
    std::vector<TransformState> captureSelectedTransforms();

    // --- Accessors ----------------------------------------------------------
    kWorld        *getWorld()        { return world; }
    kAssetManager *getAssetManager() { return world ? world->getAssetManager() : nullptr; }

    // --- Edit actions -------------------------------------------------------
    void selectAll();
    void deselectAll();
    void invertSelection();

    // --- Object creation ----------------------------------------------------
    void createSceneObject();
    void createEmpty();
    void createMeshPrimitive(kMesh *mesh, const kString &name);
    void createMeshFromFile();
    void createLight(kLightType type);
    void createCamera();

    // --- Asset creation -----------------------------------------------------
    void createNewMaterial();
    void deleteAssets(const std::vector<fs::path> &paths);

    kString getCurrentDirPath();

    void openFolder(kString name);
    void closeFolder();

    bool newProject();
    bool openProject();

    void checkAssetChange();

    void refreshWindowTitle();
    void closeEditor();

    void clearWorld(bool forced = false);
    void deleteObjectRecursive(kObject *node);

    // Editor path and directory
    fs::path exePath;
    fs::path baseDir;

    // Project info
    kString projectName;
    bool projectOpened = false;
    bool projectSaved = true;

    // Project path and directory
    fs::path projectPath;
    std::vector<kString> currentDir;

    // World info
    kString worldName = "";
    // kString worldUuid = "";

    PanelProject *panelProject;
    PanelHierarchy *panelHierarchy;

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

    std::vector<ImportTask>   importTasks;
    std::vector<ThumbnailTask> thumbnailQueue;
    kOffscreenRenderer        thumbnailRenderer{128, 128};

    void drawImportPopup(PanelConsole *console);
    void processThumbnailQueue(PanelConsole *console);

    // Check project files
    std::unordered_map<kString, FileInfo> fileMap; // Key = uuid
    std::unordered_map<kString, kString> uuidMap;  // Reverse lookup, key = filename

    // Check world objects
    std::unordered_map<kString, ObjectInfo> objectMap; // Key = uuid

    std::vector<kString> selectedObjects;
    void selectObject(const kString uuid, bool clearList = false);
    void deselectObject(const kString uuid);

    kObject *selectedObject = nullptr;
    kScene  *selectedScene  = nullptr;

    ImGuizmo::OPERATION manipulatorType = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE manipulatorMode = ImGuizmo::LOCAL;

    UndoRedoManager undoRedo;
    PivotMode pivotMode = PivotMode::LastSelected;

private:
    kWindow   *window;
    kWorld    *world;
    kRenderer     *renderer;
    kScene    *scene = nullptr;
    kString    initialWindowTitle;

    // int initialResizeCount = 0;

    // std::map<kString, kString> fileDirty;   // Files that need to be put into fileGUID, or refresh checksum into fileMD5, or regenerate thumbnail etc.
    // kString latestFileUuid = "";

    kString checkAssetType(const fs::path &p);
    void startBatchImport(const std::vector<ImportTask> &tasks);
};

#endif // FILEMANAGER_H
