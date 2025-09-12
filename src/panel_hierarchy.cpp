#include "panel_hierarchy.h"

using namespace kemena;

PanelHierarchy::PanelHierarchy(Manager* setManager, kAssetManager* assetManager, kWorld* setWorld)
	: root("World")
{
	manager = setManager;
	manager->panelHierarchy = this;

	kTexture2D* tex_add = assetManager->loadTexture2DFromResource("ICON_ADD_ROUND_BUTTON", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconAdd = (ImTextureRef)(intptr_t)tex_add->getTextureID();

	kTexture2D* tex_mag = assetManager->loadTexture2DFromResource("ICON_MAGNIFIER_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMag = (ImTextureRef)(intptr_t)tex_mag->getTextureID();

	world = setWorld;
}

void PanelHierarchy::deselectAll(Node& root)
{
	root.isSelected = false;
	for (auto& child : root.children)
	{
		deselectAll(*child);
	}
}

void PanelHierarchy::drawNode(Node& node, Node& root)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (node.isSelected) flags |= ImGuiTreeNodeFlags_Selected;
	if (node.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

	bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		if (!ImGui::GetIO().KeyShift)
		{
			deselectAll(root);
		}
		node.isSelected = !node.isSelected || ImGui::GetIO().KeyShift;
	}

	if (nodeOpen)
	{
		for (auto& child : node.children)
		{
			drawNode(*child, root);
		}
		ImGui::TreePop();
	}
}

void PanelHierarchy::drawHierarchyPanel(Node& root, bool* opened, bool enabled)
{
	ImGui::BeginDisabled(!enabled);

	if (ImGui::Begin("Hierarchy", opened))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));        // Background
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // Pressed

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0)); // smaller gap (2px horizontal, 0 vertical)

		// Add button
		{
			if (ImGui::ImageButton("add",
								   iconAdd,
								   ImVec2(16, 16),
								   ImVec2(0, 0), ImVec2(1, 1), // UVs
								   ImVec4(0, 0, 0, 0), // Background color
								   addTint)) // Tint color
			{
			}
			addTint = ImGui::IsItemActive() ? ImVec4(1, 1, 1, 0.5f) : ImVec4(1, 1, 1, 1);
		}

		ImGui::PopStyleVar(); // Restore spacing
		ImGui::PopStyleColor(3);

		// Put search box on the same line
		ImGui::SameLine();

		// Search bar
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, (22 - ImGui::GetFontSize()) * 0.5f));
		ImGui::PushItemWidth(-FLT_MIN);

		ImGui::BeginGroup();
		{
			float iconSize = ImGui::GetFontSize() * 0.8; // scale relative to text height
			ImVec2 cursor = ImGui::GetCursorScreenPos();

			// Draw the icon over the input box (aligned left-center)
			ImGui::GetWindowDrawList()->AddImage(
				iconMag,
				ImVec2(cursor.x + 4, cursor.y + (ImGui::GetFrameHeight() - iconSize) * 0.5f), // top-left
				ImVec2(cursor.x + 4 + iconSize, cursor.y + (ImGui::GetFrameHeight() + iconSize) * 0.5f), // bottom-right
				ImVec2(0,0), ImVec2(1,1),   // UVs
				IM_COL32_WHITE              // tint
			);

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(iconSize + 8, 3));

			// Input aligned with button height
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputTextWithHint("##search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
		}
		ImGui::EndGroup();

		ImGui::PopItemWidth();
		ImGui::PopStyleVar(2);

		ImGui::Spacing();

		// Tree view
		{
			// --- Tree view (fills remaining space) ---
			float availableHeight = ImGui::GetContentRegionAvail().y - 4; // 4 px spacing

			ImGui::BeginChild("HierarchyTree", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				if (manager->projectOpened)
				{
					drawNode(root, root);
				}
				else
				{
					std::string text = "No active world";
					float textWidth = ImGui::CalcTextSize(text.c_str()).x;
					float columnWidth = ImGui::GetColumnWidth();
					float textX = ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f; // center horizontally
					ImGui::SetCursorPosX(textX);

					ImGui::Text(text.c_str());
				}
			}
			ImGui::EndChild();
		}
	}
	ImGui::End();

	ImGui::EndDisabled();
}

void PanelHierarchy::draw(kGuiManager* gui, bool& opened, bool enabled)
{
	if (opened)
		drawHierarchyPanel(root, &opened, enabled);
}

void PanelHierarchy::refreshList()
{
	if (world->getScenes().size() > 1)
	{
		for (size_t i = 1; i < world->getScenes().size(); ++i)
		{
			kScene* scene = world->getScenes().at(i);

			// Add scene to root
			auto& sceneNode = root.children.emplace_back(std::make_unique<Node>(scene->getName()));

			// Loop through objects in the scene
			kObject* rootNode = scene->getRootNode();
			if (rootNode->getChildren().size() > 0)
			{
				for (size_t j = 0; j < rootNode->getChildren().size(); ++j)
				{
					kObject* childNode = rootNode->getChildren().at(j);
					//int icon;

					// WIP: Save the list item to the kObject?

					if (childNode->getType() == kNodeType::NODE_TYPE_OBJECT)
					{
						// Empty node
						//icon = hierarchyPane->iconIndexObject;
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_MESH)
					{
						// Mesh
						//icon = hierarchyPane->iconIndexMesh;
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_LIGHT)
					{
						// Light
						//icon = hierarchyPane->iconIndexLight;
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_CAMERA)
					{
						// Camera
						//icon = hierarchyPane->iconIndexCamera;
					}
					else
					{
					}

					sceneNode->children.push_back(std::make_unique<Node>(childNode->getName()));
				}
			}
		}
	}
}
