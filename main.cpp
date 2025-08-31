#include "kemena/kemena.h"

#include "filemanager.h"
#include "mainmenu.h"
#include "panel_world.h"
#include "panel_inspector.h"
#include "panel_hierarchy.h"
#include "panel_project.h"
#include "panel_console.h"

#include "imgui_internal.h"   // <-- required for ImGuiSettingsHandler

using namespace kemena;

const string windowTitle = "Kemena3D Studio";

// Project config
std::string projectName    = "New Game";
std::string developerName  = "My Company";
std::string projectVersion = "0.0.1";

bool showPanelWorld = true;
bool showPanelInspector = true;
bool showPanelHierarchy = true;
bool showPanelConsole = true;
bool showPanelProject = true;

bool enablePanelWorld = false;
bool enablePanelInspector = false;
bool enablePanelHierarchy = false;
bool enablePanelConsole = false;
bool enablePanelProject = false;

// This is used to save the 'show' and 'enable' window properties to layout ini file
struct PanelStateIniHandler
{
    static void* readOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
    {
        // We only care about our custom section
        if (strcmp(name, "Panels") == 0)
            return (void*)1;
        return nullptr;
    }

    static void readLine(ImGuiContext*, ImGuiSettingsHandler*, void*, const char* line)
    {
        int tmp;
        if (sscanf(line, "WorldOpened=%d", &tmp) == 1)
            showPanelWorld = (tmp != 0);
        else if (sscanf(line, "InspectorOpened=%d", &tmp) == 1)
            showPanelInspector = (tmp != 0);
        else if (sscanf(line, "HierarchyOpened=%d", &tmp) == 1)
            showPanelHierarchy = (tmp != 0);
        else if (sscanf(line, "ConsoleOpened=%d", &tmp) == 1)
            showPanelConsole = (tmp != 0);
        else if (sscanf(line, "ProjectOpened=%d", &tmp) == 1)
            showPanelProject = (tmp != 0);

        /*else if (sscanf(line, "WorldEnabled=%d", &tmp) == 1)
            enablePanelWorld = (tmp != 0);
        else if (sscanf(line, "InspectorEnabled=%d", &tmp) == 1)
            enablePanelInspector = (tmp != 0);
        else if (sscanf(line, "HierarchyEnabled=%d", &tmp) == 1)
            enablePanelHierarchy = (tmp != 0);
        else if (sscanf(line, "ConsoleEnabled=%d", &tmp) == 1)
            enablePanelConsole = (tmp != 0);
        else if (sscanf(line, "ProjectEnabled=%d", &tmp) == 1)
            enablePanelProject = (tmp != 0);*/
    }

    static void writeAll(ImGuiContext*, ImGuiSettingsHandler*, ImGuiTextBuffer* out_buf)
    {
        out_buf->appendf("[Panels]\n");
        out_buf->appendf("WorldOpened=%d\n", showPanelWorld ? 1 : 0);
        out_buf->appendf("InspectorOpened=%d\n", showPanelInspector ? 1 : 0);
        out_buf->appendf("HierarchyOpened=%d\n", showPanelHierarchy ? 1 : 0);
        out_buf->appendf("ConsoleOpened=%d\n", showPanelConsole ? 1 : 0);
        out_buf->appendf("ProjectOpened=%d\n", showPanelProject ? 1 : 0);

        /*out_buf->appendf("WorldEnabled=%d\n", enablePanelWorld ? 1 : 0);
        out_buf->appendf("InspectorEnabled=%d\n", enablePanelInspector ? 1 : 0);
        out_buf->appendf("HierarchyEnabled=%d\n", enablePanelHierarchy ? 1 : 0);
        out_buf->appendf("ConsoleEnabled=%d\n", enablePanelConsole ? 1 : 0);
        out_buf->appendf("ProjectEnabled=%d\n", enablePanelProject ? 1 : 0);*/

        out_buf->append("\n");
    }
};

void registerPanelStateHandler()
{
    ImGuiSettingsHandler ini_handler;
    ini_handler.TypeName = "Panels";
    ini_handler.TypeHash = ImHashStr("Panels");
    ini_handler.ReadOpenFn = PanelStateIniHandler::readOpen;
    ini_handler.ReadLineFn = PanelStateIniHandler::readLine;
    ini_handler.WriteAllFn = PanelStateIniHandler::writeAll;
    ini_handler.ClearAllFn = nullptr;
    ini_handler.ApplyAllFn = nullptr;

    ImGui::GetCurrentContext()->SettingsHandlers.push_back(ini_handler);
}

int main()
{
    // Create window and renderer
    kWindow* window = createWindow(1024, 768, windowTitle, true);
    kRenderer* renderer = createRenderer(window);
    renderer->setEnableScreenBuffer(true);
    renderer->setClearColor(vec4(0.2f, 0.4f, 0.6f, 1.0f));

    // Setup GUI
    kGuiManager* gui = createGuiManager(renderer);

    registerPanelStateHandler();

    hierarchy::init(gui);
    project::init(gui);
    console::init(gui);

    ImGui::LoadIniSettingsFromDisk("layout.ini");

    // File manager
    FileManager* fileManager = new FileManager(window);

    // Create the asset manager, world and scene
    kAssetManager* assetManager = createAssetManager();
    kWorld* world = createWorld(assetManager);
    kScene* scene = world->createScene("Scene");

    kCamera* camera = scene->addCamera(glm::vec3(-20.0f, 5.0f, 20.0f), glm::vec3(0.0f, 0.5f, 0.0f), kCameraType::CAMERA_TYPE_LOCKED);

    // Game loop
    kSystemEvent event;
    while (window->getRunning())
    {
        gui->processEvent(event);

        if (event.hasEvent())
        {
            if (event.getType() == K_EVENT_QUIT)
            {
                window->setRunning(false);
            }
        }

        renderer->render(scene, 0, 0, window->getWindowWidth(), window->getWindowHeight(), window->getTimer()->getDeltaTime(), false);

        gui->canvasStart();
        gui->dockSpaceStart("MainDockSpace");

        mainmenu::draw(gui, window, fileManager);

        world::draw(gui, showPanelWorld, enablePanelWorld, renderer);
        inspector::draw(gui, showPanelInspector, enablePanelInspector);
        hierarchy::draw(gui, showPanelHierarchy, enablePanelHierarchy);
        project::draw(gui, showPanelProject, enablePanelProject);
        console::draw(gui, showPanelConsole, enablePanelConsole);

        gui->dockSpaceEnd();
        gui->canvasEnd();

        window->swap();
    }

    // Clean up
    gui->destroy();
    renderer->destroy();
    window->destroy();
    return 0;
}
