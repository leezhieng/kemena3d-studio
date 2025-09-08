#ifndef PANEL_CONSOLE_H
#define PANEL_CONSOLE_H

#include "kemena/kemena.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdarg>

using namespace kemena;

class PanelConsole
{
	private:
		std::vector<std::string> consoleItems;
		char inputBuf[256];
		bool scrollToBottom = false;

	public:
	    PanelConsole(kGuiManager* gui);
		void addLog(const char* fmt, ...);
		static int textEditCallback(ImGuiInputTextCallbackData* data);
		void draw(kGuiManager* gui, bool& opened, bool enabled);
};

#endif

