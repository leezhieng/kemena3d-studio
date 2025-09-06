#include "panel_project.h"

using namespace kemena;

PanelProject::PanelProject()
    : root("Assets", "", nullptr, 0)
{
}

void PanelProject::init(Manager* setManager, kAssetManager* assetManager)
{
	manager = setManager;
	manager->panelProject = this;

	// Button icons
	kTexture2D* tex_up = assetManager->loadTexture2DFromResource("ICON_FOLDER_UP_BUTTON", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconUp = (ImTextureRef)(intptr_t)tex_up->getTextureID();

	kTexture2D* tex_add = assetManager->loadTexture2DFromResource("ICON_ADD_ROUND_BUTTON", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconAdd = (ImTextureRef)(intptr_t)tex_add->getTextureID();

	kTexture2D* tex_mag = assetManager->loadTexture2DFromResource("ICON_MAGNIFIER_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMag = (ImTextureRef)(intptr_t)tex_mag->getTextureID();

	// List and thumbnail icon
	kTexture2D* tex_thumb = assetManager->loadTexture2DFromResource("ICON_SCENE_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconThumbnail = (ImTextureRef)(intptr_t)tex_thumb->getTextureID();

	kTexture2D* tex_list = assetManager->loadTexture2DFromResource("ICON_HIERARCHY_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconList = (ImTextureRef)(intptr_t)tex_list->getTextureID();

	// File icons (built-in)
	kTexture2D* tex_folder = assetManager->loadTexture2DFromResource("ICON_FOLDER_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconFolder = (ImTextureRef)(intptr_t)tex_folder->getTextureID();

	kTexture2D* tex_text = assetManager->loadTexture2DFromResource("ICON_TEXT_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconText = (ImTextureRef)(intptr_t)tex_text->getTextureID();

	kTexture2D* tex_image = assetManager->loadTexture2DFromResource("ICON_IMAGE_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconImage = (ImTextureRef)(intptr_t)tex_image->getTextureID();

	kTexture2D* tex_script = assetManager->loadTexture2DFromResource("ICON_SCRIPT_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconScript = (ImTextureRef)(intptr_t)tex_script->getTextureID();

	kTexture2D* tex_audio = assetManager->loadTexture2DFromResource("ICON_AUDIO_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconAudio = (ImTextureRef)(intptr_t)tex_audio->getTextureID();

	kTexture2D* tex_video = assetManager->loadTexture2DFromResource("ICON_VIDEO_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconVideo = (ImTextureRef)(intptr_t)tex_video->getTextureID();

	kTexture2D* tex_model = assetManager->loadTexture2DFromResource("ICON_MODEL_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconModel = (ImTextureRef)(intptr_t)tex_model->getTextureID();

	kTexture2D* tex_prefab = assetManager->loadTexture2DFromResource("ICON_PREFAB_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconPrefab = (ImTextureRef)(intptr_t)tex_prefab->getTextureID();

	kTexture2D* tex_world = assetManager->loadTexture2DFromResource("ICON_WORLD_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconWorld = (ImTextureRef)(intptr_t)tex_world->getTextureID();

	kTexture2D* tex_material = assetManager->loadTexture2DFromResource("ICON_MATERIAL_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMaterial = (ImTextureRef)(intptr_t)tex_material->getTextureID();

	kTexture2D* tex_other = assetManager->loadTexture2DFromResource("ICON_OTHER_FILE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconOther = (ImTextureRef)(intptr_t)tex_other->getTextureID();

	root = Node("Asset", "", (ImTextureID)(intptr_t)tex_folder->getTextureID(), 0);
}

void PanelProject::deselectAll(Node& root)
{
	root.isSelected = false;
	for (auto& child : root.children)
	{
		deselectAll(*child);
	}
}

void PanelProject::drawNode(Node& node, Node& root, int level)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
	if (node.isSelected) flags |= ImGuiTreeNodeFlags_Selected;
	if (node.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

	// Draw the icon
    ImGui::Image(node.icon, ImVec2(16, 16));

    ImGui::SameLine();

	//bool nodeOpen = ImGui::TreeNodeEx(node.guid.c_str(), flags, "%s", node.name.c_str());
	bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags, "%s", node.name.c_str());

	// Detect double-click on this tree node
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		// Handle double-click
		//std::cout << "Double-clicked: " << node.name.c_str() << " ,Level:" << level << std::endl;

		if (node.type == 0)
        {
            if (level == 0)
            {
                manager->closeFolder();
                needRefreshList = true;
            }
            else if (level == 1)
            {
                manager->openFolder(node.name);
                needRefreshList = true;
            }
        }
	}

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
	    level++;

		for (auto& child : node.children)
		{
			drawNode(*child, root, level);
		}
		ImGui::TreePop();
	}
}

void PanelProject::drawProjectPanel(Node& root, bool* opened, bool enabled)
{
	ImGui::BeginDisabled(!enabled);

	if (ImGui::Begin("Project", opened))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));        // Background
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0)); // Hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));  // Pressed

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 0)); // smaller gap (2px horizontal, 0 vertical)

		// Up button
		{
			if (ImGui::ImageButton("up",
								   iconUp, // Size
								   ImVec2(16, 16),
								   ImVec2(0, 0), ImVec2(1,1), // UVs
								   ImVec4(0, 0, 0, 0), // Background color
								   upTint)) // Tint color
			{
			    // Move up directory
			    manager->closeFolder();
                needRefreshList = true;
			}
			upTint = ImGui::IsItemActive() ? ImVec4(1,1,1,0.5f) : ImVec4(1,1,1,1);
		}

		// Put next item on the same line
		ImGui::SameLine();

		// Add button
		{
			if (ImGui::ImageButton("add",
								   iconAdd,
								   ImVec2(16, 16),
								   ImVec2(0, 0), ImVec2(1,1), // UVs
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

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Smaller gap (0px horizontal, 0 vertical)

		// Search bar
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, (22 - ImGui::GetFontSize()) * 0.5f));
		ImGui::PushItemWidth(-FLT_MIN);

		ImGui::PopStyleVar(); // Restore spacing

		ImGui::BeginGroup();
		{
			float iconSize = ImGui::GetFontSize() * 0.8; // scale relative to text height
			ImVec2 cursor = ImGui::GetCursorScreenPos();
			float buttonWidth = 18.0f; // width of the list button including padding
			float searchWidth = ImGui::GetContentRegionAvail().x - buttonWidth - 15;

			// Draw the icon over the input box (aligned left-center)
			ImGui::GetWindowDrawList()->AddImage(
				iconMag,
				ImVec2(cursor.x + 4, cursor.y + (ImGui::GetFrameHeight() - iconSize) * 0.5f), // top-left
				ImVec2(cursor.x + 4 + iconSize, cursor.y + (ImGui::GetFrameHeight() + iconSize) * 0.5f), // bottom-right
				ImVec2(0,0), ImVec2(1,1),   // UVs
				IM_COL32_WHITE              // tint
			);

			// Search input
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(iconSize + 8, 3));
			ImGui::SetNextItemWidth(searchWidth);
			ImGui::InputTextWithHint("##search", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, 8.0f);

			// List or Thumbnail button
			{
				if (displayThumbnail)
				{
					if (ImGui::ImageButton("thumbnail",
										   iconThumbnail,
										   ImVec2(16, 16),
										   ImVec2(0,0), ImVec2(1,1), // UVs
										   ImVec4(0, 0, 0, 0), // Background color
										   ImVec4(1, 1, 1, 1))) // Tint color
					{
						displayThumbnail = false;
					}
				}
				else
				{
					if (ImGui::ImageButton("list",
										   iconList,
										   ImVec2(16, 16),
										   ImVec2(0,0), ImVec2(1,1), // UVs
										   ImVec4(0, 0, 0, 0), // Background color
										   ImVec4(1, 1, 1, 1))) // Tint color
					{
						displayThumbnail = true;
					}
				}
			}
		}
		ImGui::EndGroup();

		ImGui::PopItemWidth();
		ImGui::PopStyleVar();

		ImGui::Spacing();
		ImGui::Spacing();

		// Tree View
		{
			// --- Tree view (fills remaining space) ---
			float availableHeight = ImGui::GetContentRegionAvail().y - 4; // 4 px spacing

			// Child region with border for drawNode
			ImGui::BeginChild("ProjectTree", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				drawNode(root, root);
			}
			ImGui::EndChild();
		}

		ImGui::Spacing();
	}
	ImGui::End();

	ImGui::EndDisabled();
}

void PanelProject::draw(kGuiManager* gui, bool& opened, bool enabled)
{
	if (opened)
    {
        if (needRefreshList)
            refreshList();

        drawProjectPanel(root, &opened, enabled);
    }
}

void PanelProject::refreshList()
{
	if (manager->projectOpened)
	{
		fs::path path = manager->projectPath;
		if (!manager->currentDir.empty())
		{
			for (const auto& dir : manager->currentDir)
			{
				path /= dir;  // appends with correct separator for the platform
			}
		}

		if (fs::is_directory(path))
		{
			std::string lastFolder = path.filename().string();
			//std::cout << "Last folder: " << lastFolder << "\n";

            // Reset root safely
			root.name = lastFolder;
            root.guid = "";
            root.icon = iconFolder;
            root.type = 0;
            root.isSelected = false;
            root.children.clear();
		}

		// Loop through all the folders and files in path
		if (!fs::exists(path))
		{
			std::cerr << "Path does not exist: " << path << "\n";
			return;
		}

		// Do directory first
		{
		    if (!fs::is_empty(path))
            {
                for (const auto& entry : fs::directory_iterator(path))
                {
                    if (entry.is_directory())
                    {
                        //std::cout << "[DIR]  " << entry.path().filename().string() << "\n";

                        // Add to list
                        root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconFolder, 0));
                    }
                }
            }
		}

		// Do files
		{
		    if (!fs::is_empty(path))
            {
                for (const auto& entry : fs::directory_iterator(path))
                {
                    if (entry.is_regular_file())
                    {
                        //std::cout << "[FILE] " << entry.path().filename().string() << "\n";

                        auto ext = entry.path().extension().string();

                        // Add to list
                        if (ext == ".txt" || ext == ".json")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconText, 1));
                        else if (ext == ".jpg" || ext == ".png" || ext == ".tga")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconImage, 1));
                        else if (ext == ".as")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconScript, 1));
                        else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconAudio, 1));
                        else if (ext == ".mp4" || ext == ".avi" || ext == ".mov")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconVideo, 1));
                        else if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".glb")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconModel, 1));
                        else if (ext == ".pfb")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconPrefab, 1));
                        else if (ext == ".world")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconWorld, 1));
                        else if (ext == ".mat")
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconMaterial, 1));
                        else
                            root.children.emplace_back(std::make_unique<Node>(entry.path().filename().string(), "", iconOther, 1));
                    }
                    else
                    {
                        // Don't display anything if it's not a folder or file (could be symlink, drive, etc.)
                    }
                }
            }
		}
	}
}

