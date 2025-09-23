#include "panel_world.h"

PanelWorld::PanelWorld(kGuiManager* setGuiManager, Manager* setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

void PanelWorld::draw(bool& isOpened, kRenderer* renderer, kCamera* editorCamera)
{
    enabled = manager->projectOpened;

    if (!isOpened || renderer == nullptr || editorCamera == nullptr)
        return;

    ImGui::BeginDisabled(!enabled);
    ImGui::Begin("World");

    // Panel size
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 availSize = ImGui::GetContentRegionAvail();
    width = (int)availSize.x;
    height = (int)availSize.y;
    aspectRatio = (height > 0.0f) ? (availSize.x / availSize.y) : 1.0f;

    // Absolute screen coordinates
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    ImVec2 panelPos  = ImVec2(windowPos.x + contentMin.x, windowPos.y + contentMin.y);
    ImVec2 panelSize = ImVec2(contentMax.x - contentMin.x, contentMax.y - contentMin.y);

    // Display framebuffer texture
    ImTextureID tex_id = (ImTextureID)(intptr_t)renderer->getFboTexture();
    ImGui::Image(tex_id, availSize, ImVec2(0,1), ImVec2(1,0));

    ImGui::SetItemAllowOverlap();

    hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    // Only manipulate if a selected object exists
    if (manager->selectedObject)
    {
        // Copy the object's world matrix
        glm::mat4 model = manager->selectedObject->getModelMatrixWorld();

        // Get camera matrices
        glm::mat4 view = editorCamera->getViewMatrix();
        glm::mat4 proj = editorCamera->getProjectionMatrix();

        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

        //std::cout << glm::value_ptr(view) << "," << glm::value_ptr(proj) << "," << glm::value_ptr(model) << std::endl;

        ImGuizmo::BeginFrame();

        // Must call this, otherwise will crash
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

        // Setup ImGuizmo for the panel
        ImGuizmo::SetRect(panelPos.x, panelPos.y, panelSize.x, panelSize.y);

        // Manipulate the matrix copy
        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), operation, mode, glm::value_ptr(model));

        // If user interacted, decompose and apply back
        if (ImGuizmo::IsUsing())
        {
            glm::vec3 position, scale, skew;
            glm::quat rotation;
            glm::vec4 perspective;

            if (glm::decompose(model, scale, rotation, position, skew, perspective))
            {
                manager->selectedObject->setPosition(position);
                manager->selectedObject->setRotation(rotation);
                manager->selectedObject->setScale(scale);
            }

            //std::cout << "Is dragging" << std::endl;
        }

        if (ImGuizmo::IsOver())
        {
            //std::cout << "Is over" << std::endl;
        }
    }

    ImGui::End();
    ImGui::EndDisabled();
}
