#ifndef PANEL_WORLD_H
#define PANEL_WORLD_H

#include <algorithm>

#include "kemena/kemena.h"

using namespace kemena;

namespace panelWorld
{
    bool enabled = false;
    bool hovered = false;
    bool focused = false;

    void draw(kGuiManager* gui, bool& isOpened, bool isEnabled, kRenderer* renderer)
    {
        enabled = isEnabled;

        ImGui::BeginDisabled(!enabled);

        ImGui::Begin("World");

        // (Optional) allow user to size the preview
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 preview = ImVec2(avail.x, avail.y); // fill

        // Display the texture. NOTE: flip V (uv0.y=1, uv1.y=0)
        ImTextureID tex_id = (ImTextureID)(intptr_t)renderer->getFboTexture(); // ImGui OpenGL backend expects this cast
        ImGui::Image(tex_id, preview, ImVec2(0,1), ImVec2(1,0)); // flip Y so it appears right-side-up

        hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
        focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

        ImGui::End();

        ImGui::EndDisabled();
    }
}

#endif
