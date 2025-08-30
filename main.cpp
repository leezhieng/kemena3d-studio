#include "kemena/kemena.h"

#include "filemanager.h"
#include "mainmenu.h"
#include "panel_world.h"
#include "panel_inspector.h"
#include "panel_hierarchy.h"
#include "panel_project.h"
#include "panel_console.h"

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

	// Setup GUI
	kGuiManager* gui = createGuiManager(renderer);
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

		world::draw(gui, renderer);
		inspector::draw(gui);
		hierarchy::draw(gui);
		project::draw(gui);
		console::draw(gui);

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
