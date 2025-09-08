#include "panel_console.h"

PanelConsole::PanelConsole(kGuiManager* gui)
{
    addLog("Welcome to Kemena3D Studio. Create or open a project to get started.", inputBuf);
}

void PanelConsole::addLog(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	va_end(args);
	consoleItems.push_back(std::string(buf));
	scrollToBottom = true;
}

int PanelConsole::textEditCallback(ImGuiInputTextCallbackData* data)
{
	// For history browsing or autocompletion if needed
	return 0;
}

void PanelConsole::draw(kGuiManager* gui, bool& opened, bool enabled)
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
		if (scrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		scrollToBottom = false;
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
