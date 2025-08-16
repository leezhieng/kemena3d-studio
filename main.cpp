#include "kemena/kemena.h"

#include "MainMenu.h"

#include "Panel_Inspector.h"
#include "Panel_Hierarchy.h"
#include "Panel_Project.h"
#include "Panel_Console.h"

using namespace kemena;

int main()
{
	// Create window and renderer
	kWindow* window = createWindow(1024, 768, "Kemena3D Studio", true);
	kRenderer* renderer = createRenderer(window);
	renderer->setClearColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	// Create the asset manager, world and scene
	kAssetManager* assetManager = createAssetManager();
	kWorld* world = createWorld(assetManager);
	kScene* scene = world->createScene("My Scene");

	kCamera* camera = scene->addCamera(glm::vec3(-20.0f, 5.0f, 20.0f), glm::vec3(0.0f, 0.5f, 0.0f), kCameraType::CAMERA_TYPE_LOCKED);

	kGuiManager* gui = createGuiManager(renderer);

	// Setup GUI
	hierarchy::init(gui);
	project::init(gui);

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

		//renderer->render(scene, 0, 0, window->getWindowWidth(), window->getWindowHeight(), window->getTimer()->getDeltaTime(), false);

		gui->canvasStart();
		gui->dockSpaceStart("MainDockSpace");

		mainmenu::draw(gui);
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
