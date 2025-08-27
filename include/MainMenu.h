#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "kemena/kemena.h"

#include <SDL3/SDL_dialog.h>

using namespace kemena;

namespace mainmenu
{
	static void SDLCALL saveWorkspaceCallback(void* userdata, const char* const* filelist, int filter)
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

	static void SDLCALL loadWorkspaceCallback(void* userdata, const char* const* filelist, int filter)
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
			ImGui::LoadIniSettingsFromDisk(path);
		}
	}

	void draw(kGuiManager* gui, kWindow* window, FileManager* file)
	{
		if (gui->menuBar())
		{
			// File menu
			if (gui->menu("File"))
			{
				if (gui->menuItem("New Scene", "", false, false)) {}
				if (gui->menuItem("Open Scene", "", false, false)) {}
				if (gui->menuItem("Open Recent Scene", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Save", "", false, false)) {}
				if (gui->menuItem("Save As...", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("New Project", "")) { file->newProject(); }
				if (gui->menuItem("Open Project", "")) { file->openProject(); }
				if (gui->menuItem("Save Project", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Build Settings", "", false, false)) {}
				if (gui->menuItem("Build And Run", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Exit")) {}

				gui->menuEnd();
			}

			// Edit menu
			if (gui->menu("Edit"))
			{
				if (gui->menuItem("Undo", "Ctrl+Z", false, false)) {}
				if (gui->menuItem("Redo", "Ctrl+Y", false, false)) {}
				if (gui->menuItem("Undo History", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Select All", "", false, false)) {}
				if (gui->menuItem("Deselect All", "", false, false)) {}
				if (gui->menuItem("Select Children", "", false, false)) {}
				if (gui->menuItem("Select Prefab Root", "", false, false)) {}
				if (gui->menuItem("Invert Selection", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Cut", "Ctrl+X", false, false)) {}
				if (gui->menuItem("Copy", "Ctrl+C", false, false)) {}
				if (gui->menuItem("Paste", "Ctrl+V", false, false)) {}
				if (gui->menuItem("Paste As Child", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Frame Selected", "", false, false)) {}
				if (gui->menuItem("Lock View To Selected", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Play", "", false, false)) {}
				if (gui->menuItem("Pause", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Sign In", "")) {}
				if (gui->menuItem("Sign Out", "")) {}
				gui->separator();
				if (gui->menuItem("Project Settings", "", false, false)) {}
				if (gui->menuItem("Preferences", "")) {}
				if (gui->menuItem("Shortcuts", "")) {}
				if (gui->menuItem("Clear All PlayerPrefs", "", false, false)) {}

				gui->menuEnd();
			}

			// Assets Menu
			if (gui->menu("Assets"))
			{
				if (gui->menuItem("Create", "", false, false)) {}
				if (gui->menuItem("Show In Explorer", "", false, false)) {}
				if (gui->menuItem("Open", "", false, false)) {}
				if (gui->menuItem("Delete", "", false, false)) {}
				if (gui->menuItem("Rename", "", false, false)) {}
				if (gui->menuItem("Copy Path", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Refresh", "", false, false)) {}
				if (gui->menuItem("Reimport", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Reimport All", "", false, false)) {}
				gui->separator();
				if (gui->menuItem("Generate Lighting", "", false, false)) {}

				gui->menuEnd();
			}

			// Object Menu
			if (gui->menu("Object"))
			{
				if (gui->menuItem("Create Empty Parent", "", false, false)) {}
				if (gui->menuItem("Create Empty Child", "", false, false)) {}
				if (gui->menuItem("Create Empty", "", false, false)) {}
				if (gui->menuItem("3D Object", "", false, false)) {}
				if (gui->menuItem("Effects", "", false, false)) {}
				if (gui->menuItem("Light", "", false, false)) {}
				if (gui->menuItem("Audio", "", false, false)) {}
				if (gui->menuItem("Video", "", false, false)) {}
				if (gui->menuItem("UI", "", false, false)) {}
				if (gui->menuItem("Camera", "", false, false)) {}

				gui->menuEnd();
			}

			// Component Menu
			if (gui->menu("Component"))
			{
				if (gui->menuItem("Audio", "", false, false)) {}
				if (gui->menuItem("Effect", "", false, false)) {}
				if (gui->menuItem("Mesh", "", false, false)) {}
				if (gui->menuItem("Physics", "", false, false)) {}
				if (gui->menuItem("Scripts", "", false, false)) {}

				gui->menuEnd();
			}

			// Window menu
			if (gui->menu("Window"))
			{
				if (gui->menu("General"))
				{
					static bool showInspector = true;
					if (gui->menuItem("Inspector", "", showInspector))
						showInspector = !showInspector;

					static bool showHierarchy = true;
					if (gui->menuItem("Hierarchy", "", showHierarchy))
						showHierarchy = !showHierarchy;

					static bool showProject = true;
					if (gui->menuItem("Project", "", showProject))
						showProject = !showProject;

					static bool showScript = true;
					if (gui->menuItem("Script", "", showScript))
						showScript = !showScript;

					static bool showConsole = true;
					if (gui->menuItem("Console", "", showConsole))
						showConsole = !showConsole;

					gui->menuEnd();
				}

				static bool showRendering = true;
				if (gui->menuItem("Rendering", "", showRendering))
					showRendering = !showRendering;

				static bool showAnimation = true;
				if (gui->menuItem("Animation", "", showAnimation))
					showAnimation = !showAnimation;

				static bool showAudio = true;
				if (gui->menuItem("Audio", "", showAudio))
					showAudio = !showAudio;

				static bool showSequencing = true;
				if (gui->menuItem("Sequencing", "", showSequencing))
					showSequencing = !showSequencing;

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
							"layout.ini",          // default start file/dir
							false
						);
					}
					gui->separator();
					if (gui->menuItem("Reset", ""))
					{
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
				if (gui->menuItem("Manual", "")) {}
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
}

#endif

