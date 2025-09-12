#ifndef PANEL_CONSOLE_H
#define PANEL_CONSOLE_H

#include "kemena/kemena.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdarg>

using namespace kemena;

enum class LogLevel { Info, Warning, Error };

struct ConsoleItem
{
	std::string text;
	LogLevel level;
};

class PanelConsole
{
	private:
		std::vector<ConsoleItem> consoleItems;
		char inputBuf[256];
		bool scrollToBottom = false;

		std::unordered_set<int> selectedIndices; // all selected lines
        int lastClickedIndex = -1;               // for shift selection

        bool showCopiedTooltip = false;
        float copiedTooltipTime = 0.0f; // in seconds
        const float copiedTooltipDuration = 1.0f; // show 1 second

	public:
		PanelConsole(kGuiManager* gui);
		void addLog(LogLevel level, const char* fmt, ...);
		static int textEditCallback(ImGuiInputTextCallbackData* data);
		void draw(kGuiManager* gui, bool& opened, bool enabled);
};

#endif

