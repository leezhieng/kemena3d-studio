#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "kemena/kemena.h"

#include "manager.h"

#include <SDL3/SDL_dialog.h>

#include "imgui_internal.h"   // Required for ImGuiSettingsHandler
#include "stb/stb_image.h"

#include <windows.h>

using namespace kemena;

struct ShowPanel
{
	bool world = true;
	bool inspector = true;
	bool hierarchy = true;
	bool console = true;
	bool project = true;
};

inline ShowPanel showPanel;
inline ImGuiSettingsHandler ini_handler;
inline bool isReloadLayout = false;
inline std::string layoutFileName = "layout.ini";

class MainMenu
{
	public:
		MainMenu();
		static void SDLCALL saveWorkspaceCallback(void* userdata, const char* const* filelist, int filter);
		static void SDLCALL loadWorkspaceCallback(void* userdata, const char* const* filelist, int filter);
		void draw(kGuiManager* gui, kWindow* window, Manager* manager, ShowPanel& showPanel);

		static void* readOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);
		static void readLine(ImGuiContext*, ImGuiSettingsHandler*, void*, const char* line);
		static void writeAll(ImGuiContext*, ImGuiSettingsHandler*, ImGuiTextBuffer* out_buf);
		static void registerPanelStateHandler();
};

#endif

