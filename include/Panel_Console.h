#ifndef PANEL_CONSOLE_H
#define PANEL_CONSOLE_H

#include "kemena/kemena.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdarg>

using namespace kemena;

namespace panelConsole
{
	static std::vector<std::string> consoleItems;
	static char inputBuf[256];
	static bool ScrollToBottom = false;

	void addLog(const char* fmt, ...)
	{
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
		va_end(args);
		consoleItems.push_back(std::string(buf));
		ScrollToBottom = true;
	}

	static int textEditCallback(ImGuiInputTextCallbackData* data)
	{
		// For history browsing or autocompletion if needed
		return 0;
	}

	void init(kGuiManager* gui)
	{
	    addLog("Welcome to Kemena3D Studio. Create or open a project to get started.", inputBuf);
	}

	void draw(kGuiManager* gui, bool& opened, bool enabled)
	{
		if (opened)
		{
		    if (!enabled)
                ImGui::BeginDisabled(true);

			ImGui::Begin("Console", &opened);

			// Main log display
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
			for (auto& item : consoleItems)
			{
				ImGui::TextUnformatted(item.c_str());
			}
			if (ScrollToBottom)
				ImGui::SetScrollHereY(1.0f);
			ScrollToBottom = false;
			ImGui::EndChild();

			// Command input
			ImGui::Separator();
			ImGui::PushItemWidth(-1);
			if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf),
								 ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
								 &textEditCallback))
			{
				if (inputBuf[0])
				{
					addLog("> %s", inputBuf);
					// Handle command here
					if (strcmp(inputBuf, "clear") == 0)
						consoleItems.clear();
					else if (strcmp(inputBuf, "help") == 0)
						addLog("Available commands: help, clear");
					else
						addLog("Unknown command: '%s'", inputBuf);

					strcpy(inputBuf, "");
				}
			}
			ImGui::PopItemWidth();

			ImGui::End();

			if (!enabled)
                ImGui::EndDisabled();
		}
	}
}

#endif

