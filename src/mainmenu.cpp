#include "mainmenu.h"

MainMenu::MainMenu(kGuiManager* setGuiManager, Manager* setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

void MainMenu::SDLCALL saveWorkspaceCallback(void* userdata, const char* const* filelist, int filter)
{
	if (!filelist)
	{
		SDL_Log("Save dialog error: %s", SDL_GetError());
	}
	else if (!*filelist)
	{
		SDL_Log("Save dialog cancelled");
	}
	else
	{
		const char* path = filelist[0];
		SDL_Log("Saving layout to: %s", path);
		ImGui::SaveIniSettingsToDisk(path);
	}
}

void SDLCALL MainMenu::loadWorkspaceCallback(void* userdata, const char* const* filelist, int filter)
{
	if (!filelist)
	{
		SDL_Log("Load dialog error: %s", SDL_GetError());
	}
	else if (!*filelist)
	{
		SDL_Log("Load dialog cancelled");
	}
	else
	{
		const char* path = filelist[0];
		SDL_Log("Loading layout from: %s", path);
		//ImGui::LoadIniSettingsFromDisk(path);

		isReloadLayout = true;
		layoutFileName = path;
	}
}

void MainMenu::draw(kWindow* window, ShowPanel& showPanel)
{
	if (gui->menuBar())
	{
		// File menu
		if (gui->menu("File"))
		{
			if (gui->menuItem("New World", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Open World", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Open Recent World", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Save", "Ctrl+S", false, manager->projectOpened)) {}
			if (gui->menuItem("Save As...", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("New Project", ""))
			{
				manager->newProject();
			}
			if (gui->menuItem("Open Project", ""))
			{
				manager->openProject();
			}
			if (gui->menuItem("Save Project", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Build Settings", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Build And Run", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Exit")) { manager->closeEditor(); }

			gui->menuEnd();
		}

		// Edit menu
		if (gui->menu("Edit"))
		{
			if (gui->menuItem("Undo", "Ctrl+Z", false, manager->projectOpened)) {}
			if (gui->menuItem("Redo", "Ctrl+Y", false, manager->projectOpened)) {}
			if (gui->menuItem("Undo History", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Select All", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Deselect All", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Select Children", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Select Prefab Root", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Invert Selection", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Cut", "Ctrl+X", false, manager->projectOpened)) {}
			if (gui->menuItem("Copy", "Ctrl+C", false, manager->projectOpened)) {}
			if (gui->menuItem("Paste", "Ctrl+V", false, manager->projectOpened)) {}
			if (gui->menuItem("Paste As Child", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Frame Selected", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Lock View To Selected", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Play", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Pause", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Sign In", "")) {}
			if (gui->menuItem("Sign Out", "")) {}
			gui->separator();
			if (gui->menuItem("Project Settings", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Preferences", "")) {}
			if (gui->menuItem("Shortcuts", "")) {}
			if (gui->menuItem("Clear All PlayerPrefs", "", false, manager->projectOpened)) {}

			gui->menuEnd();
		}

		// Assets Menu
		if (gui->menu("Assets"))
		{
			if (gui->menuItem("Create", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Show In Explorer", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Open", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Delete", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Rename", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Copy Path", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Refresh", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Reimport", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Reimport All", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Generate Lighting", "", false, manager->projectOpened)) {}

			gui->menuEnd();
		}

		// Object Menu
		if (gui->menu("Object"))
		{
			if (gui->menuItem("Create Scene", "", false, manager->projectOpened)) {}
			gui->separator();
			if (gui->menuItem("Create Empty Parent", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Create Empty Child", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Create Empty", "", false, manager->projectOpened)) {}
			if (gui->menuItem("3D Object", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Effects", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Light", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Audio", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Video", "", false, manager->projectOpened)) {}
			if (gui->menuItem("UI", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Camera", "", false, manager->projectOpened)) {}

			gui->menuEnd();
		}

		// Component Menu
		if (gui->menu("Component"))
		{
			if (gui->menuItem("Audio", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Effect", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Mesh", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Physics", "", false, manager->projectOpened)) {}
			if (gui->menuItem("Scripts", "", false, manager->projectOpened)) {}

			gui->menuEnd();
		}

		// Window menu
		if (gui->menu("Window"))
		{
			if (gui->menu("General"))
			{
				if (gui->menuItem("Inspector", "", showPanel.inspector))
					showPanel.inspector = !showPanel.inspector;

				if (gui->menuItem("Hierarchy", "", showPanel.hierarchy))
					showPanel.hierarchy = !showPanel.hierarchy;

				if (gui->menuItem("Project", "", showPanel.project))
					showPanel.project = !showPanel.project;

				//if (gui->menuItem("Script", "", showScript))
				//showScript = !showScript;

				if (gui->menuItem("Console", "", showPanel.console))
					showPanel.console = !showPanel.console;

				gui->menuEnd();
			}

			//if (gui->menuItem("Rendering", "", showRendering))
			//showRendering = !showRendering;

			//if (gui->menuItem("Animation", "", showAnimation))
			//showAnimation = !showAnimation;

			//if (gui->menuItem("Audio", "", showAudio))
			//showAudio = !showAudio;

			//if (gui->menuItem("Sequencing", "", showSequencing))
			//showSequencing = !showSequencing;

			gui->separator();

			if (gui->menu("Workspace"))
			{
				if (gui->menuItem("Save", ""))
				{
					SDL_DialogFileFilter filters[] =
					{
						{ "Ini files", "ini" },
						{ "All files", "*" }
					};
					SDL_ShowSaveFileDialog(
						saveWorkspaceCallback,
						nullptr,              // userdata
						window->getSdlWindow(),
						filters,
						SDL_arraysize(filters),
						"layout.ini"          // default filename
					);
				}
				if (gui->menuItem("Load", ""))
				{
					SDL_DialogFileFilter filters[] =
					{
						{ "Ini files", "ini" },
						{ "All files", "*" }
					};
					SDL_ShowOpenFileDialog(
						loadWorkspaceCallback,
						nullptr,              // userdata
						window->getSdlWindow(),
						filters,
						SDL_arraysize(filters),
						"layout.ini",          // default start manager/dir
						false
					);
				}
				gui->separator();
				if (gui->menuItem("Reset", ""))
				{
					isReloadLayout = true;
					layoutFileName = "layout.ini";
				}

				gui->menuEnd();
			}

			gui->menuEnd();
		}

		// Help Menu
		if (gui->menu("Help"))
		{
			if (gui->menuItem("About", "")) {}
			gui->separator();
			if (gui->menuItem("Manual", ""))
			{
				SDL_OpenURL("https://kemena3d.com/manual");
			}
			if (gui->menuItem("Scripting Reference", "")) {}
			gui->separator();
			if (gui->menuItem("Release Notes", "")) {}
			if (gui->menuItem("Software Licenses", "")) {}
			if (gui->menuItem("Report a Bug", "")) {}

			gui->menuEnd();
		}

		gui->menuBarEnd();
	}
}

void* MainMenu::readOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
{
	// We only care about our custom section
	if (strcmp(name, "Panels") == 0)
		return (void*)1;
	return nullptr;
}

void MainMenu::readLine(ImGuiContext*, ImGuiSettingsHandler*, void*, const char* line)
{
	int tmp;
	if (sscanf(line, "WorldOpened=%d", &tmp) == 1)
		showPanel.world = (tmp != 0);
	else if (sscanf(line, "InspectorOpened=%d", &tmp) == 1)
		showPanel.inspector = (tmp != 0);
	else if (sscanf(line, "HierarchyOpened=%d", &tmp) == 1)
		showPanel.hierarchy = (tmp != 0);
	else if (sscanf(line, "ConsoleOpened=%d", &tmp) == 1)
		showPanel.console = (tmp != 0);
	else if (sscanf(line, "ProjectOpened=%d", &tmp) == 1)
		showPanel.project = (tmp != 0);
}

void MainMenu::writeAll(ImGuiContext*, ImGuiSettingsHandler*, ImGuiTextBuffer* out_buf)
{
	out_buf->appendf("[Panels]\n");
	out_buf->appendf("WorldOpened=%d\n", showPanel.world ? 1 : 0);
	out_buf->appendf("InspectorOpened=%d\n", showPanel.inspector ? 1 : 0);
	out_buf->appendf("HierarchyOpened=%d\n", showPanel.hierarchy ? 1 : 0);
	out_buf->appendf("ConsoleOpened=%d\n", showPanel.console ? 1 : 0);
	out_buf->appendf("ProjectOpened=%d\n", showPanel.project ? 1 : 0);

	out_buf->append("\n");
}

void MainMenu::registerPanelStateHandler()
{
	ini_handler.TypeName = "Panels";
	ini_handler.TypeHash = ImHashStr("Panels");
	ini_handler.ReadOpenFn = readOpen;
	ini_handler.ReadLineFn = readLine;
	ini_handler.WriteAllFn = writeAll;
	ini_handler.ClearAllFn = nullptr;
	ini_handler.ApplyAllFn = nullptr;

	ImGui::GetCurrentContext()->SettingsHandlers.push_back(ini_handler);
}

