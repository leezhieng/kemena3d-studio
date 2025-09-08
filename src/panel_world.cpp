#include "panel_world.h"

void PanelWorld::draw(kGuiManager* gui, bool& isOpened, bool isEnabled, kRenderer* renderer, kCamera* editorCamera)
{
	enabled = isEnabled;

	ImGui::BeginDisabled(!enabled);

	ImGui::Begin("World");

	// (Optional) allow user to size the preview
	ImVec2 avail = ImGui::GetContentRegionAvail();
	ImVec2 preview = ImVec2(avail.x, avail.y); // fill

	// Update render size and projection aspect ratio
	width = (int)preview.x;
	height = (int)preview.y;
	aspectRatio = (preview.y > 0.0f) ? (preview.x / preview.y) : 1.0f;

	// Display the texture. NOTE: flip V (uv0.y=1, uv1.y=0)
	ImTextureID tex_id = (ImTextureID)(intptr_t)renderer->getFboTexture(); // ImGui OpenGL backend expects this cast
	ImGui::Image(tex_id, preview, ImVec2(0,1), ImVec2(1,0)); // flip Y so it appears right-side-up

	hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
	focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

	ImGui::End();

	ImGui::EndDisabled();
}
