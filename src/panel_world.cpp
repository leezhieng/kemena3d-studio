#include "panel_world.h"

PanelWorld::PanelWorld(kGuiManager* setGuiManager, Manager* setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

void PanelWorld::draw(bool& isOpened, kRenderer* renderer, kCamera* editorCamera)
{
	enabled = manager->projectOpened;

	ImGui::BeginDisabled(!enabled);
	ImGui::Begin("World");

	// (Optional) allow user to size the preview
	ImVec2 panelSize = ImGui::GetContentRegionAvail();
    // Update render size and projection aspect ratio
	width = (int)panelSize.x;
	height = (int)panelSize.y;
	aspectRatio = (panelSize.y > 0.0f) ? (panelSize.x / panelSize.y) : 1.0f;

	//std::cout << width << "," << height << "," << aspectRatio << std::endl;

	// Display the texture. NOTE: flip V (uv0.y=1, uv1.y=0)
	ImTextureID tex_id = (ImTextureID)(intptr_t)renderer->getFboTexture(); // ImGui OpenGL backend expects this cast
	ImGui::Image(tex_id, panelSize, ImVec2(0,1), ImVec2(1,0)); // Flip Y so it appears right-side-up

	hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
	focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	ImGui::End();
	ImGui::EndDisabled();
}
