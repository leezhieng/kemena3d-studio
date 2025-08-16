#ifndef PANEL_INSPECTOR_H
#define PANEL_INSPECTOR_H

#include "kemena/kemena.h"

using namespace kemena;

namespace inspector
{
	bool opened = true;

	void draw(kGuiManager* gui)
	{
		if (opened)
		{
			gui->windowStart("Inspector", &opened);

			// --- Icon & Right-side group ---
			{
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
			}

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

			gui->windowEnd();
		}
	}
}

#endif

