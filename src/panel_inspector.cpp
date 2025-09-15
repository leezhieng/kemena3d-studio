#include "panel_inspector.h"

PanelInspector::PanelInspector(kGuiManager* setGuiManager, Manager* setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

void PanelInspector::draw(bool& opened)
{
	if (opened)
	{
		if (!manager->projectOpened)
			ImGui::BeginDisabled(true);

		gui->windowStart("Inspector", &opened);

		if (manager->selectedObjects.size() > 0)
		{
		    if (manager->selectedObjects.size() == 1)
            {
                // Only 1 object selected

                // Icon on the left
                gui->groupStart();
                gui->button("Icon", ivec2(48, 48)); // Replace with ImGui::Image for real icon
                gui->groupEnd();

                gui->sameLine();

                // Right side (name + checkboxes)
                gui->groupStart();

                // Name field
                static char name[128] = "My Object";
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputText("##Name", name, IM_ARRAYSIZE(name));

                // Visible + Static in the same row
                static bool enabled = true;
                static bool isStatic = false;
                gui->checkbox("Enabled", &enabled);
                gui->sameLine();
                gui->checkbox("Static", &isStatic);

                gui->groupEnd();

                gui->spacing();

                // --- Transform section ---
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    static float position[3] = {0.0f, 0.0f, 0.0f};
                    static float rotation[3] = {0.0f, 0.0f, 0.0f};
                    static float scale[3]    = {1.0f, 1.0f, 1.0f};

                    if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingStretchProp))
                    {
                        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                        // Position
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Position");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::DragFloat3("##Position", position, 0.1f);

                        // Rotation
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Rotation");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::DragFloat3("##Rotation", rotation, 0.1f);

                        // Scale
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Scale");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetNextItemWidth(-FLT_MIN);
                        ImGui::DragFloat3("##Scale", scale, 0.1f);

                        ImGui::EndTable();
                    }
                }
            }
            else
            {
                // More than 1 object selected

            }
		}
		else
		{
			gui->spacing();

			std::string text = "Nothing is selected";
            float textWidth = ImGui::CalcTextSize(text.c_str()).x;
            float columnWidth = ImGui::GetColumnWidth();
            float textX = ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f; // center horizontally
            ImGui::SetCursorPosX(textX);

            ImGui::Text(text.c_str());
		}

		gui->windowEnd();

		if (!manager->projectOpened)
			ImGui::EndDisabled();
	}
}


