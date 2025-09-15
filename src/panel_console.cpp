#include "panel_console.h"

PanelConsole::PanelConsole(kGuiManager* setGuiManager, Manager* setManager)
{
	addLog(LogLevel::Info, "Welcome to Kemena3D Studio. Create or open a project to get started.", inputBuf);

	gui = setGuiManager;
	manager = setManager;
}

void PanelConsole::addLog(LogLevel level, const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	va_end(args);

	ConsoleItem item;
	item.text = std::string(buf);
	item.level = level;

	consoleItems.push_back(item);
	scrollToBottom = true;
}

int PanelConsole::textEditCallback(ImGuiInputTextCallbackData* data)
{
	// For history browsing or autocompletion if needed
	return 0;
}

void PanelConsole::draw(bool& opened)
{
	if (opened)
	{
		if (!manager->projectOpened)
			ImGui::BeginDisabled(true);

		ImGui::Begin("Console", &opened);

		ImGuiIO& io = ImGui::GetIO();

		// Main log display
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (consoleItems.size() > 0)
		{
		    for (size_t i = 0; i < consoleItems.size(); ++i)
            {
                auto& item = consoleItems[i];
                ImVec4 color;
                switch (item.level)
                {
                    case LogLevel::Info:
                        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                        break;  // white
                    case LogLevel::Warning:
                        color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                        break;  // yellow
                    case LogLevel::Error:
                        color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
                        break;  // red
                }

                ImGui::PushStyleColor(ImGuiCol_Text, color);

                bool selected = selectedIndices.count(i) > 0;

                if (ImGui::Selectable((item.text + "##ConsoleMessage" + std::to_string(i)).c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowOverlap))
                {
                    bool shift = ImGui::GetIO().KeyShift;
                    bool ctrl  = ImGui::GetIO().KeyCtrl;

                    if (shift && lastClickedIndex >= 0)
                    {
                        // Shift: select range from last clicked to current
                        size_t start = std::min(lastClickedIndex, i);
                        size_t end   = std::max(lastClickedIndex, i);
                        for (size_t j = start; j <= end; ++j)
                            selectedIndices.insert(j);
                    }
                    else if (ctrl)
                    {
                        // Ctrl: toggle selection
                        if (selected)
                            selectedIndices.erase(i);
                        else
                            selectedIndices.insert(i);
                    }
                    else
                    {
                        // Normal click: select only this
                        selectedIndices.clear();
                        selectedIndices.insert(i);
                    }

                    lastClickedIndex = i;

                    // Copy to clipboard on double click
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        ImGui::SetClipboardText(item.text.c_str());

                        showCopiedTooltip = true;
                        copiedTooltipTime = 0.0f; // reset timer
                    }
                }

                // Right-click context menu for this line
                if (ImGui::BeginPopupContextItem(string("##ConsoleMessage" + std::to_string(i)).c_str()))
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

                    if (ImGui::MenuItem("Select This"))
                        selectedIndices.insert(i);

                    if (ImGui::MenuItem("Select All"))
                        for (size_t j = 0; j < consoleItems.size(); ++j)
                            selectedIndices.insert(j);

                    // Deselect only the current line
                    if (ImGui::MenuItem("Deselect This"))
                        selectedIndices.erase(i);

                    if (ImGui::MenuItem("Deselect All"))
                        selectedIndices.clear();

                    if (ImGui::MenuItem("Copy Text"))
                    {
                        std::string textToCopy;
                        if (selectedIndices.empty())
                            textToCopy = item.text;
                        else
                            for (int idx : selectedIndices)
                                textToCopy += consoleItems[idx].text + "\n";

                        ImGui::SetClipboardText(textToCopy.c_str());
                        showCopiedTooltip = true;
                        copiedTooltipTime = 0.0f; // reset timer
                    }

                    ImGui::PopStyleColor();
                    ImGui::EndPopup();
                }

                ImGui::PopStyleColor();
            }
		}
		if (scrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		scrollToBottom = false;
		ImGui::EndChild();

		// Command input
		ImGui::Separator();
		ImGui::PushItemWidth(-1);
		if (ImGui::InputText("##ConsoleInput", inputBuf, IM_ARRAYSIZE(inputBuf),
							 ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
							 &textEditCallback))
		{
			if (inputBuf[0])
			{
				addLog(LogLevel::Info, "> %s", inputBuf);
				// Handle command here
				if (strcmp(inputBuf, "clear") == 0)
					consoleItems.clear();
				else if (strcmp(inputBuf, "help") == 0)
					addLog(LogLevel::Info, "Available commands: help, clear");
				else
					addLog(LogLevel::Warning, "Unknown command: '%s'", inputBuf);

				strcpy(inputBuf, "");
			}
		}
		ImGui::PopItemWidth();

		ImGui::End();

		if (!manager->projectOpened)
			ImGui::EndDisabled();

		// Show tooltip
		if (showCopiedTooltip)
		{
			copiedTooltipTime += io.DeltaTime;
			ImGui::BeginTooltip();
			ImGui::Text("Text copied to clipboard");
			ImGui::EndTooltip();

			if (copiedTooltipTime >= copiedTooltipDuration)
			{
				showCopiedTooltip = false;
			}
		}
	}
}
