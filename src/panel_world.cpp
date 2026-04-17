#include "panel_world.h"

PanelWorld::PanelWorld(kGuiManager *setGuiManager, Manager *setManager)
{
    gui     = setGuiManager;
    manager = setManager;

    // Disable the hatched/dashed overlay on gizmo axis lines
    ImGuizmo::GetStyle().HatchedAxisLineThickness = 0.0f;
}

// ---------------------------------------------------------------------------
// Pivot toolbar helpers
// ---------------------------------------------------------------------------

static void pivotButton(const char *label, PivotMode mode, PivotMode &current)
{
    bool active = (current == mode);
    if (active)
    {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.85f));
    }
    if (ImGui::Button(label, ImVec2(26, 22)))
        current = mode;
    if (active)
        ImGui::PopStyleColor(2);
}

// ---------------------------------------------------------------------------
// draw
// ---------------------------------------------------------------------------

void PanelWorld::draw(bool &isOpened, kRenderer *renderer, kCamera *editorCamera)
{
    enabled = manager->projectOpened;

    if (!isOpened || renderer == nullptr || editorCamera == nullptr)
        return;

    ImGui::BeginDisabled(!enabled);
    ImGui::Begin("World");

    // ------------------------------------------------------------------
    // Pivot mode toolbar (shown above the scene image)
    // ------------------------------------------------------------------
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

    pivotButton("I", PivotMode::Individual,   manager->pivotMode);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Individual pivot");
    ImGui::SameLine();
    pivotButton("C", PivotMode::Center,       manager->pivotMode);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Center pivot");
    ImGui::SameLine();
    pivotButton("L", PivotMode::LastSelected, manager->pivotMode);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Last selected pivot");

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(8, 0));
    ImGui::SameLine();

    // Render mode selector
    static const char *kRenderModeNames[] = {
        "Full", "Albedo", "Normals", "Wireframe", "Depth", "Full+Wire"
    };
    int currentMode = (int)renderer->getRenderMode();
    ImGui::SetNextItemWidth(110.0f);
    if (ImGui::Combo("##RenderMode", &currentMode, kRenderModeNames, 6))
        renderer->setRenderMode((kRenderMode)currentMode);

    ImGui::PopStyleVar();

    ImGui::Separator();

    // ------------------------------------------------------------------
    // Panel layout
    // ------------------------------------------------------------------
    ImVec2 availSize  = ImGui::GetContentRegionAvail();
    width       = (int)availSize.x;
    height      = (int)availSize.y;
    aspectRatio = (height > 0.0f) ? (availSize.x / availSize.y) : 1.0f;

    // Use the actual cursor position so panelPos aligns with the image,
    // not the window content-region start (which is above the toolbar).
    panelPos         = ImGui::GetCursorScreenPos();
    ImVec2 panelSize = availSize;

    // Display framebuffer texture
    ImTextureRef tex_ref((ImTextureID)(uintptr_t)renderer->getFboTexture());
    ImGui::SetNextItemAllowOverlap();
    ImGui::Image(tex_ref, availSize, ImVec2(0, 1), ImVec2(1, 0));

    hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

    // ------------------------------------------------------------------
    // Multi-object gizmo
    // ------------------------------------------------------------------
    if (!manager->selectedObjects.empty())
    {
        // Gather valid selected object pointers
        std::vector<kObject *> selObjs;
        for (const auto &uuid : manager->selectedObjects)
        {
            kObject *obj = manager->findObjectByUuid(uuid);
            if (obj && obj->getActive()) selObjs.push_back(obj);
        }

        if (!selObjs.empty())
        {
            glm::mat4 view = editorCamera->getViewMatrix();
            glm::mat4 proj = editorCamera->getProjectionMatrix();

            // Compute pivot matrix
            glm::mat4 pivotMatrix;
            if (manager->pivotMode == PivotMode::Center)
            {
                glm::vec3 center(0.0f);
                for (kObject *obj : selObjs)
                    center += obj->getPosition();
                center /= (float)selObjs.size();
                pivotMatrix = glm::translate(glm::mat4(1.0f), center);
            }
            else
            {
                // LastSelected or Individual → pivot at last selected
                kObject *pivot = manager->selectedObject ? manager->selectedObject : selObjs.back();
                pivotMatrix = pivot->getModelMatrixWorld();
            }

            glm::mat4 pivotCopy = pivotMatrix;

            ImGuizmo::BeginFrame();
            ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
            ImGuizmo::SetRect(panelPos.x, panelPos.y, panelSize.x, panelSize.y);

            ImGuizmo::Manipulate(
                glm::value_ptr(view), glm::value_ptr(proj),
                manager->manipulatorType, manager->manipulatorMode,
                glm::value_ptr(pivotCopy));

            bool isUsingNow = ImGuizmo::IsUsing();

            // Snapshot start state when drag begins
            if (!wasGizmoUsing && isUsingNow)
                gizmoStartStates = manager->captureSelectedTransforms();

            if (isUsingNow)
            {
                glm::mat4 delta = pivotCopy * glm::inverse(pivotMatrix);

                for (kObject *obj : selObjs)
                {
                    if (selObjs.size() == 1)
                    {
                        // pivotMatrix started as this object's world matrix,
                        // so pivotCopy IS the new world matrix — use it directly.
                        glm::vec3 pos, scale, skew;
                        glm::quat rot;
                        glm::vec4 persp;
                        glm::decompose(pivotCopy, scale, rot, pos, skew, persp);
                        obj->setPosition(pos);
                        obj->setRotation(glm::normalize(rot));
                        obj->setScale(scale);
                    }
                    else if (manager->pivotMode == PivotMode::Individual)
                    {
                        // Each object around its own centre — extract pure deltas
                        // from the pivot matrices to avoid the T*R*T^-1 drift.
                        glm::vec3 dPos = glm::vec3(pivotCopy[3]) - glm::vec3(pivotMatrix[3]);

                        glm::vec3 sOld(glm::length(glm::vec3(pivotMatrix[0])),
                                       glm::length(glm::vec3(pivotMatrix[1])),
                                       glm::length(glm::vec3(pivotMatrix[2])));
                        glm::vec3 sNew(glm::length(glm::vec3(pivotCopy[0])),
                                       glm::length(glm::vec3(pivotCopy[1])),
                                       glm::length(glm::vec3(pivotCopy[2])));

                        glm::mat3 rOld(glm::vec3(pivotMatrix[0]) / sOld.x,
                                       glm::vec3(pivotMatrix[1]) / sOld.y,
                                       glm::vec3(pivotMatrix[2]) / sOld.z);
                        glm::mat3 rNew(glm::vec3(pivotCopy[0]) / sNew.x,
                                       glm::vec3(pivotCopy[1]) / sNew.y,
                                       glm::vec3(pivotCopy[2]) / sNew.z);
                        glm::quat dRot = glm::normalize(glm::quat_cast(rNew * glm::transpose(rOld)));

                        obj->setPosition(obj->getPosition() + dPos);
                        obj->setRotation(glm::normalize(dRot * obj->getRotation()));
                        obj->setScale(obj->getScale() * (sNew / sOld));
                    }
                    else
                    {
                        // Center / LastSelected: apply full delta to world matrix
                        glm::mat4 newWorld = delta * obj->getModelMatrixWorld();
                        glm::vec3 pos, scale, skew;
                        glm::quat rot;
                        glm::vec4 persp;
                        glm::decompose(newWorld, scale, rot, pos, skew, persp);
                        obj->setPosition(pos);
                        obj->setRotation(glm::normalize(rot));
                        obj->setScale(scale);
                    }
                }
            }

            // Push undo command when drag ends
            if (wasGizmoUsing && !isUsingNow)
            {
                auto after = manager->captureSelectedTransforms();
                manager->undoRedo.push(std::make_unique<TransformCommand>(
                    manager, gizmoStartStates, std::move(after)));
            }

            wasGizmoUsing = isUsingNow;
        }
    }

    ImGui::End();
    ImGui::EndDisabled();
}
