#include "panel_hierarchy.h"

using namespace kemena;

PanelHierarchy::PanelHierarchy(kGuiManager* setGuiManager, Manager* setManager, kAssetManager* assetManager, kWorld* setWorld)
	: root("World", "world", nullptr, "world")
{
    gui = setGuiManager;
	manager = setManager;
	manager->panelHierarchy = this;

	kTexture2D* tex_add = assetManager->loadTexture2DFromResource("ICON_ADD_ROUND_BUTTON", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconAdd = (ImTextureRef)(intptr_t)tex_add->getTextureID();

	kTexture2D* tex_mag = assetManager->loadTexture2DFromResource("ICON_MAGNIFIER_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMag = (ImTextureRef)(intptr_t)tex_mag->getTextureID();

	// Object type icons
    kTexture2D* tex_world = assetManager->loadTexture2DFromResource("ICON_OBJECT_WORLD", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconWorld = (ImTextureRef)(intptr_t)tex_world->getTextureID();

	kTexture2D* tex_scene = assetManager->loadTexture2DFromResource("ICON_OBJECT_SCENE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconScene = (ImTextureRef)(intptr_t)tex_scene->getTextureID();

	kTexture2D* tex_mesh = assetManager->loadTexture2DFromResource("ICON_OBJECT_MESH", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMesh = (ImTextureRef)(intptr_t)tex_mesh->getTextureID();

	kTexture2D* tex_empty = assetManager->loadTexture2DFromResource("ICON_OBJECT_EMPTY", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconEmpty = (ImTextureRef)(intptr_t)tex_empty->getTextureID();

	kTexture2D* tex_light = assetManager->loadTexture2DFromResource("ICON_OBJECT_LIGHT", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconLight = (ImTextureRef)(intptr_t)tex_light->getTextureID();

	kTexture2D* tex_camera = assetManager->loadTexture2DFromResource("ICON_OBJECT_CAMERA", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconCamera = (ImTextureRef)(intptr_t)tex_camera->getTextureID();

	kTexture2D* tex_prefab = assetManager->loadTexture2DFromResource("ICON_OBJECT_PREFAB", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconPrefab = (ImTextureRef)(intptr_t)tex_prefab->getTextureID();

	world = setWorld;

	root = Node("World", "world", iconWorld, "world");
}

void PanelHierarchy::deselectAll(Node& root)
{
	root.isSelected = false;
	for (auto& child : root.children)
	{
		deselectAll(*child);
	}
}

void PanelHierarchy::drawNode(Node& node, Node& root, int level)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (node.isSelected) flags |= ImGuiTreeNodeFlags_Selected;
	if (node.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

	// Only expand world and scene by default
	if (level <= 1)
	{
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	// Draw the icon
	ImGui::Image(node.icon, ImVec2(16, 16));

	ImGui::SameLine();

	bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

	// Item clicked
	if (ImGui::IsItemClicked())
	{
		if (!ImGui::GetIO().KeyShift)
		{
			deselectAll(root);
		}
		node.isSelected = !node.isSelected || ImGui::GetIO().KeyShift;

		if (ImGui::GetIO().KeyShift)
            manager->selectObject(node.uuid, false);
        else
            manager->selectObject(node.uuid, true);

        std::cout << "Object clicked: " << node.uuid.c_str() << " ,Level:" << level << std::endl;

        if (level == 0)
        {
            // World
        }
        else if (level == 1)
        {
            // Scene
        }
        else
        {
            // Objects
            if (manager->objectMap[node.uuid.c_str()].object != nullptr)
            {
                std::cout << "FOUND: " << node.uuid.c_str() << std::endl;
                manager->selectedObject = manager->objectMap[node.uuid.c_str()].object;
            }
            else
            {
                std::cout << "NOT FOUND: " << node.uuid.c_str() << std::endl;
            }
        }
	}

	if (nodeOpen)
	{
	    level++;

		for (auto& child : node.children)
		{
			drawNode(*child, root, level);
		}
		ImGui::TreePop();
	}
}

void PanelHierarchy::drawHierarchyPanel(Node& root, bool* opened)
{
	ImGui::BeginDisabled(!manager->projectOpened);

	if (ImGui::Begin("Hierarchy", opened))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));        // Background
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // Pressed

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0)); // smaller gap (2px horizontal, 0 vertical)

		// Add button
		{
			if (ImGui::ImageButton("AddButton",
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
			ImGui::InputTextWithHint("##SearchHierarchy", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
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
					drawNode(root, root, 0);
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

void PanelHierarchy::draw(bool& opened)
{
	if (opened)
		drawHierarchyPanel(root, &opened);
}

void PanelHierarchy::refreshList()
{
    manager->objectMap.clear();

	if (world->getScenes().size() > 1)
	{
		for (size_t i = 1; i < world->getScenes().size(); ++i)
		{
			kScene* scene = world->getScenes().at(i);

			// Add scene to root
			auto& sceneNode = root.children.emplace_back(std::make_unique<Node>(scene->getName(), scene->getUuid(), iconScene, "scene"));

			// Loop through objects in the scene
			kObject* rootNode = scene->getRootNode();
			if (rootNode->getChildren().size() > 0)
			{
				for (size_t j = 0; j < rootNode->getChildren().size(); ++j)
				{
					kObject* childNode = rootNode->getChildren().at(j);
					ImTextureRef icon;
					string type = "";

					// WIP: Save the list item to the kObject?

					if (childNode->getType() == kNodeType::NODE_TYPE_OBJECT)
					{
						// Empty node
						icon = iconEmpty;
						type = "object";
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_MESH)
					{
						// Mesh
						icon = iconMesh;
						type = "mesh";
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_LIGHT)
					{
						// Light
						icon = iconLight;
						type = "light";
					}
					else if (childNode->getType() == kNodeType::NODE_TYPE_CAMERA)
					{
						// Camera
						icon = iconCamera;
						type = "camera";
					}
					else
					{
					    icon = iconEmpty;
						type = "unknown";
					}

					sceneNode->children.push_back(std::make_unique<Node>(childNode->getName(), childNode->getUuid(), icon, type));

					// Put into object map
					ObjectInfo info;
					info.object = childNode;

					manager->objectMap[childNode->getUuid()] = info;
				}
			}
		}
	}
}
