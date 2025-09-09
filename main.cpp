#include "kemena/kemena.h"

#include "manager.h"
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

int main()
{
    // Create window and renderer
    kWindow* window = createWindow(1024, 768, windowTitle, true);
    kRenderer* renderer = createRenderer(window);
    renderer->setEnableScreenBuffer(true);
    renderer->setClearColor(vec4(0.2f, 0.4f, 0.6f, 1.0f));

    // Setup GUI manager
    kGuiManager* gui = createGuiManager(renderer);

    // Create the asset manager
    kAssetManager* assetManager = createAssetManager();

    // Switch default font
    gui->loadDefaultFontFromResource("FONT_OPENSANS");

    // Create the world and scene
    kWorld* world = createWorld(assetManager);
    kScene* sceneEditor = world->createScene("Editor Scene");
    kScene* scene = world->createScene("Scene");

    // Editor manager
    Manager* manager = new Manager(window, world);

    // Initialize panels
    MainMenu* mainmenu = new MainMenu();
    PanelWorld* panelWorld = new PanelWorld();
    PanelInspector* panelInspector = new PanelInspector();
    PanelProject* panelProject = new PanelProject(manager, assetManager);
    PanelHierarchy* panelHierarchy = new PanelHierarchy(manager, assetManager, world);
    PanelConsole* panelConsole = new PanelConsole(gui);

    // Load default editor layout
    mainmenu->registerPanelStateHandler();
    ImGui::LoadIniSettingsFromDisk("layout.ini");

    // Default skybox
    kShader* skyShader = assetManager->loadShaderFromResource("SHADER_VERTEX_SKYBOX", "SHADER_FRAGMENT_SKYBOX");
    kMaterial* skyMaterial = assetManager->createMaterial(skyShader);
    kTextureCube* skyTexture = assetManager->loadTextureCubeFromResource("TEXTURE_SKYBOX_LEFT",
                                                             "TEXTURE_SKYBOX_RIGHT",
                                                             "TEXTURE_SKYBOX_TOP",
                                                             "TEXTURE_SKYBOX_BOTTOM",
                                                             "TEXTURE_SKYBOX_FRONT",
                                                             "TEXTURE_SKYBOX_BACK",
                                                             "cubeMap");
    skyMaterial->addTexture(skyTexture);
    skyMaterial->setSingleSided(false);
    //kMesh* skyMesh = assetManager->loadMeshFromResource("MODEL_SHAPE_CUBE", "obj");
    kMesh* skyMesh = assetManager->loadMesh("D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shape/cube.obj");
    skyMesh->setMaterial(skyMaterial);
    scene->setSkybox(skyMaterial, skyMesh);

    // Editor grid
    //kMesh* gridMesh = assetManager->loadMeshFromResource("MODEL_SHAPE_PLANE", "obj");
    kMesh* gridMesh = sceneEditor->addMesh("D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shape/plane.obj");
    sceneEditor->addMesh(gridMesh);
    kShader* gridShader = assetManager->loadShaderFromResource("SHADER_VERTEX_GRID", "SHADER_FRAGMENT_GRID");
    //kShader* gridShader = assetManager->loadShaderFromFile("D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shader/glsl/grid.vert", "D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shader/glsl/grid.frag");
    kMaterial* gridMat = assetManager->createMaterial(gridShader);
    gridMat->setTransparent(kTransparentType::TRANSP_TYPE_BLEND);
    gridMat->setSingleSided(false);
    gridMesh->setMaterial(gridMat);

    //kMesh* test = assetManager->loadMeshFromResource("MODEL_SHAPE_CUBE", "obj");
    kMesh* test = assetManager->loadMesh("D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shape/cube.obj");
    test->setName("test");
    test->setPosition(vec3(0.0f, 1.0f, 0.0f));
    scene->addMesh(test);
    kShader* testShader = assetManager->loadShaderFromResource("SHADER_VERTEX_FLAT", "SHADER_FRAGMENT_FLAT");
    //kShader* testShader = assetManager->loadShaderFromFile("D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shader/glsl/flat.vert", "D:/Projects/Kemena3D/kloena-kemena3d-playground/assets/shader/glsl/flat.frag");
    kMaterial* testMaterial = assetManager->createMaterial(testShader);
    test->setMaterial(testMaterial);


    // Editor camera
    kCamera* cameraEditor = sceneEditor->addCamera(glm::vec3(0.0f, 5.0f, 10.0f), glm::vec3(0.0f, 2.5f, 0.0f), kCameraType::CAMERA_TYPE_FREE);
    cameraEditor->setFOV(60.0f);
    scene->setMainCamera(cameraEditor);

    bool dragging = false;
    vec2 dragStart;
    quat camRot;

    // Game loop
    kSystemEvent event;
    while (window->getRunning())
    {
        // Must reset the layout at the beginning of the frame
        if (isReloadLayout)
        {
            showPanel = ShowPanel();
            ImGui::LoadIniSettingsFromDisk(layoutFileName.c_str());

            isReloadLayout = false;
        }

        float deltaTime = window->getTimer()->getDeltaTime();
        gui->processEvent(event);

        // Event
        if (event.hasEvent())
        {
            int eventType = event.getType();

            if (eventType == K_EVENT_QUIT)
            {
                window->setRunning(false);
            }
            else if (eventType == K_EVENT_MOUSEBUTTONDOWN)
            {
                if (panelWorld->enabled && panelWorld->hovered)
                {
                    if (event.getMouseButton() == K_MOUSEBUTTON_LEFT)
                    {
                        dragging = true;

                        dragStart.x = event.getMouseX();
                        dragStart.y = event.getMouseY();

                        camRot = cameraEditor->getRotation();
                    }
                }
                else
                {
                    dragging = false;
                }
            }
            else if (eventType == K_EVENT_MOUSEBUTTONUP)
            {
                if (dragging)
                    dragging = false;

                if (panelWorld->enabled && panelWorld->hovered)
                {
                    if (event.getMouseButton() == K_MOUSEBUTTON_LEFT)
                    {
                        camRot = cameraEditor->getRotation();
                    }
                }
            }
            else if (eventType == K_EVENT_MOUSEMOTION)
            {
                if (panelWorld->enabled && panelWorld->hovered)
                {
                    if (dragging)
                    {
                        float deltaX = dragStart.x - event.getMouseX();  // horizontal mouse movement
                        float deltaY = dragStart.y - event.getMouseY();  // vertical mouse movement

                        if (cameraEditor->getCameraType() == kCameraType::CAMERA_TYPE_FREE)
                        {
                            cameraEditor->rotateByMouse(camRot, -deltaX, -deltaY);
                        }
                    }
                }
            }
            else if (eventType == K_EVENT_MOUSEWHEEL)
            {
                if (panelWorld->enabled && panelWorld->hovered)
                {
                    cameraEditor->setPosition(cameraEditor->getPosition() + cameraEditor->calculateForward() * 2.0f * event.getMouseWheelY());
                }
            }
            else if (eventType == K_EVENT_KEYDOWN)
            {
                if (event.getKeyButton() == K_KEY_1)
                {
                    //cameraEditor->setCameraType(kCameraType::CAMERA_TYPE_FREE);
                }
                else if (event.getKeyButton() == K_KEY_2)
                {
                    //cameraEditor->setCameraType(kCameraType::CAMERA_TYPE_LOCKED);
                }
            }
        }

        if (panelWorld->enabled && panelWorld->focused)
        {
            if (event.getKeyDown(K_KEY_W))
            {
                cameraEditor->setPosition(cameraEditor->getPosition() + cameraEditor->calculateForward() * deltaTime * 10.0f);
            }
            else if (event.getKeyDown(K_KEY_S))
            {
                cameraEditor->setPosition(cameraEditor->getPosition() + cameraEditor->calculateForward() * deltaTime * -10.0f);
            }
            else if (event.getKeyDown(K_KEY_A))
            {
                cameraEditor->setPosition(cameraEditor->getPosition() + cameraEditor->calculateRight() * deltaTime * 10.0f);
            }
            else if (event.getKeyDown(K_KEY_D))
            {
                cameraEditor->setPosition(cameraEditor->getPosition() + cameraEditor->calculateRight() * deltaTime * -10.0f);
            }
        }

        renderer->clear();

        // Fix aspect ratio
        if (panelWorld->width > 0 && panelWorld->height > 0)
        {
            cameraEditor->setAspectRatio(panelWorld->aspectRatio);

            renderer->render(scene, 0, 0, panelWorld->width, panelWorld->height, window->getTimer()->getDeltaTime(), false);
            renderer->render(sceneEditor, 0, 0, panelWorld->width, panelWorld->height, window->getTimer()->getDeltaTime(), false);
        }

        gui->canvasStart();
        gui->dockSpaceStart("MainDockSpace");

        mainmenu->draw(gui, window, manager, showPanel);

        panelWorld->draw(gui, showPanel.world, manager->projectOpened, renderer, cameraEditor);
        panelInspector->draw(gui, showPanel.inspector, manager->projectOpened);
        panelHierarchy->draw(gui, showPanel.hierarchy, manager->projectOpened);
        panelProject->draw(gui, showPanel.project, manager->projectOpened);
        panelConsole->draw(gui, showPanel.console, manager->projectOpened);

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
