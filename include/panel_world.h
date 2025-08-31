#ifndef PANEL_WORLD_H
#define PANEL_WORLD_H

#include <algorithm>

#include "kemena/kemena.h"

using namespace kemena;

namespace world
{
    void draw(kGuiManager* gui, bool& opened, bool enabled, kRenderer* renderer)
    {
        if (!enabled)
            ImGui::BeginDisabled(true);

        ImGui::Begin("World");

        // (Optional) allow user to size the preview
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 preview = ImVec2(avail.x, avail.y); // fill

        // Display the texture. NOTE: flip V (uv0.y=1, uv1.y=0)
        ImTextureID tex_id = (ImTextureID)(intptr_t)renderer->getFboTexture(); // ImGui OpenGL backend expects this cast
        ImGui::Image(tex_id, preview, ImVec2(0,1), ImVec2(1,0)); // flip Y so it appears right-side-up

        ImGui::End();

        if (!enabled)
            ImGui::EndDisabled();
    }
}

#endif
