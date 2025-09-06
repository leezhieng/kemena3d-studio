#ifndef DATATYPE_H
#define DATATYPE_H

#include "imgui_internal.h"   // <-- required for ImGuiSettingsHandler
#include "stb/stb_image.h"

#include <windows.h>

struct ShowPanel
{
    bool world = true;
    bool inspector = true;
    bool hierarchy = true;
    bool console = true;
    bool project = true;
};

ShowPanel showPanel;
ImGuiSettingsHandler ini_handler;
bool isReloadLayout = false;
std::string layoutFileName = "layout.ini";

static void* readOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name)
    {
        // We only care about our custom section
        if (strcmp(name, "Panels") == 0)
            return (void*)1;
        return nullptr;
    }

    static void readLine(ImGuiContext*, ImGuiSettingsHandler*, void*, const char* line)
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

    static void writeAll(ImGuiContext*, ImGuiSettingsHandler*, ImGuiTextBuffer* out_buf)
    {
        out_buf->appendf("[Panels]\n");
        out_buf->appendf("WorldOpened=%d\n", showPanel.world ? 1 : 0);
        out_buf->appendf("InspectorOpened=%d\n", showPanel.inspector ? 1 : 0);
        out_buf->appendf("HierarchyOpened=%d\n", showPanel.hierarchy ? 1 : 0);
        out_buf->appendf("ConsoleOpened=%d\n", showPanel.console ? 1 : 0);
        out_buf->appendf("ProjectOpened=%d\n", showPanel.project ? 1 : 0);

        out_buf->append("\n");
    }

static void registerPanelStateHandler()
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

#endif

