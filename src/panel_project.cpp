#include "panel_project.h"

using namespace kemena;

PanelProject::PanelProject(Manager* setManager, kAssetManager* assetManager)
	: rootTree("Assets", "", nullptr, 0), rootThumbnail("Assets", "", nullptr, 0)
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

	rootTree = Node("Asset", "", (ImTextureID)(intptr_t)tex_folder->getTextureID(), 0);
	rootThumbnail = Node("Asset", "", (ImTextureID)(intptr_t)tex_folder->getTextureID(), 0);
}

void PanelProject::deselectAll(Node& root)
{
	root.isSelected = false;
	for (auto& child : root.children)
	{
		deselectAll(*child);
	}
}

void PanelProject::drawProjectPanel(Node& rootTree, Node& rootThumbnail, bool* opened, bool enabled)
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
					if (ImGui::ImageButton("list",
										   iconList,
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
					if (ImGui::ImageButton("thumbnail",
										   iconThumbnail,
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

		// --- Tree view (fills remaining space) ---
		float availableHeight = ImGui::GetContentRegionAvail().y - 4; // 4 px spacing

		// Tree View
		if (displayThumbnail)
		{
			// Child region with border for drawTreeNode
			ImGui::BeginChild("ProjectThumbnail", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
			    drawBreadcrumb();
				drawThumbnailNode(rootThumbnail);
			}
			ImGui::EndChild();
		}
		else
		{
			// Child region with border for drawTreeNode
			ImGui::BeginChild("ProjectTree", ImVec2(0, availableHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
			{
				drawTreeNode(rootTree, rootTree);
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
		{
			refreshTreeList();
			refreshThumbnailList();

			needRefreshList = false;
		}

		drawProjectPanel(rootTree, rootThumbnail, &opened, enabled);
	}
}

void PanelProject::refreshTreeList()
{
	if (!manager->projectOpened)
		return;

	fs::path path = manager->projectPath;
	path /= "Assets"; // Tree view always show the full project structure

	if (!fs::exists(path) || !fs::is_directory(path))
	{
		std::cerr << "Path does not exist or is not a directory: " << path << "\n";
		return;
	}

	// Root node setup
	std::string lastFolder = path.filename().string();
	rootTree.name = lastFolder;
	rootTree.uuid = "";
	rootTree.icon = iconFolder;
	rootTree.type = 0;
	rootTree.isSelected = false;
	rootTree.children.clear();

	// Fill recursively
	populateTree(rootTree, path);
}

void PanelProject::drawTreeNode(Node& node, Node& rootTree, int level)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (node.isSelected) flags |= ImGuiTreeNodeFlags_Selected;
	if (node.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

	// Only expand the first node by default
	if (&node == &rootTree)
	{
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	// Draw the icon
	ImGui::Image(node.icon, ImVec2(16, 16));

	ImGui::SameLine();

	//bool nodeOpen = ImGui::TreeNodeEx(node.uuid.c_str(), flags, "%s", node.name.c_str());
	bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags, "%s", node.name.c_str());

	// Detect double-click on this tree node
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		// Handle double-click
		//std::cout << "Double-clicked: " << node.name.c_str() << " ,Level:" << level << std::endl;

		// Folder
		if (node.type == 0)
		{
			// Toggle open/close
			// WIP

			if (level == 0)
			{
				//manager->closeFolder();
				//needRefreshList = true;
			}
			else if (level == 1)
			{
				//manager->openFolder(node.name);
				//needRefreshList = true;
			}
		}
	}

	if (ImGui::IsItemClicked())
	{
		if (!ImGui::GetIO().KeyShift)
		{
			deselectAll(rootTree);
		}
		node.isSelected = !node.isSelected || ImGui::GetIO().KeyShift;
	}

	if (nodeOpen)
	{
		level++;

		for (auto& child : node.children)
		{
			drawTreeNode(*child, rootTree, level);
		}
		ImGui::TreePop();
	}
}

void PanelProject::populateTree(Node& parent, const fs::path& path)
{
	if (fs::is_empty(path))
		return;

	// --- First: Directories ---
	for (const auto& entry : fs::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			auto child = std::make_unique<Node>(
							 entry.path().filename().string(),
							 "",
							 iconFolder,
							 0
						 );

			// Recursively fill subfolder
			populateTree(*child, entry.path());

			parent.children.emplace_back(std::move(child));
		}
	}

	// --- Second: Files ---
	for (const auto& entry : fs::directory_iterator(path))
	{
		if (entry.is_regular_file())
		{
			auto ext = entry.path().extension().string();
			ImTextureRef icon;

			if (ext == ".txt" || ext == ".ini" || ext == ".xml" || ext == ".json")
				icon = iconText;
			else if (ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".png" || ext == ".gif" || ext == ".tiff" || ext == ".tga")
				icon = iconImage;
			else if (ext == ".as")
				icon = iconScript;
			else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
				icon = iconAudio;
			else if (ext == ".mp4" || ext == ".mov" || ext == ".avi" || ext == ".webm")
				icon = iconVideo;
			else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".dae" || ext == ".stl")
				icon = iconModel;
			else if (ext == ".pfb")
				icon = iconPrefab;
			else if (ext == ".world")
				icon = iconWorld;
			else if (ext == ".mat")
				icon = iconMaterial;
			else
				icon = iconOther;

			parent.children.emplace_back(std::make_unique<Node>(
											 entry.path().filename().string(),
											 "",
											 icon,
											 1
										 ));
		}
	}
}

void PanelProject::refreshThumbnailList()
{
	if (manager->projectOpened)
	{
		fs::path path = manager->projectPath;
		if (!manager->currentDir.empty())
		{
			for (const auto& dir : manager->currentDir)
				path /= dir;  // appends with correct separator for the platform
		}

		if (!fs::exists(path) || !fs::is_directory(path))
		{
			std::cerr << "Path does not exist or is not a directory: " << path << "\n";
			return;
		}

		if (fs::is_directory(path))
		{
			std::string lastFolder = path.filename().string();
			//std::cout << "Last folder: " << lastFolder << "\n";
			//std::cout << "Last folder: " << path << "\n";

			// Check whether uuid already exist for this folder
			// Generate a uuid if haven't, then save to the list

			// Reset root safely
			rootThumbnail.name = lastFolder;
			rootThumbnail.uuid = "";
			rootThumbnail.icon = iconFolder;
			rootThumbnail.type = 0;
			rootThumbnail.isSelected = false;
			rootThumbnail.children.clear();
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
			    // Do for folders
			    {
                    for (const auto& entry : fs::directory_iterator(path))
                    {
                        if (entry.is_directory())
                        {
                            rootThumbnail.children.emplace_back(std::make_unique<Node>(
                                                                    entry.path().filename().string(),
                                                                    "",
                                                                    iconFolder,
                                                                    0
                                                                ));
                        }
                    }
			    }

			    // Do for files
			    {
                    for (const auto& entry : fs::directory_iterator(path))
                    {
                        if (entry.is_regular_file())
                        {
                            auto ext = entry.path().extension().string();
                            ImTextureRef icon;

                            if (ext == ".txt" || ext == ".ini" || ext == ".xml" || ext == ".json")
                                icon = iconText;
                            else if (ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".png" || ext == ".gif" || ext == ".tiff" || ext == ".tga")
                                icon = iconImage;
                            else if (ext == ".as")
                                icon = iconScript;
                            else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
                                icon = iconAudio;
                            else if (ext == ".mp4" || ext == ".mov" || ext == ".avi" || ext == ".webm")
                                icon = iconVideo;
                            else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".dae" || ext == ".stl")
                                icon = iconModel;
                            else if (ext == ".pfb")
                                icon = iconPrefab;
                            else if (ext == ".world")
                                icon = iconWorld;
                            else if (ext == ".mat")
                                icon = iconMaterial;
                            else
                                icon = iconOther;

                            rootThumbnail.children.emplace_back(std::make_unique<Node>(
                                                                    entry.path().filename().string(),
                                                                    "",
                                                                    icon,
                                                                    1
                                                                ));
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
}

void PanelProject::drawThumbnailNode(const Node& currentDir)
{
	float thumbSize   = 48.0f;   // icon + padding
	float cellPadding = 8.0f;   // spacing between columns
	float cellWidth   = thumbSize + cellPadding;

	float panelWidth  = ImGui::GetContentRegionAvail().x;
	int columns = (int)(panelWidth / cellWidth);
	if (columns < 1) columns = 1; // at least one column

	ImGui::Columns(columns, nullptr, false);

	for (auto& child : currentDir.children)
	{
		// Draw icon
		ImGui::Image(child->icon, ImVec2(thumbSize, thumbSize));

		// Double-click to open folder
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			// Folder
			if (child->type == 0)
			{
				manager->openFolder(child->name);
				needRefreshList = true;

				ImGui::Columns(1); // reset columns before returning
				return;
			}
		}

		// Show name under icon
		ImGui::TextWrapped("%s", child->name.c_str());

		ImGui::NextColumn();
	}

	ImGui::Columns(1); // reset
}

void PanelProject::drawBreadcrumb()
{
    if (manager->currentDir.size() > 0)
    {
        // Start from project root
        fs::path basePath = manager->projectPath;

        // Draw Assets folder
        if (ImGui::Button("Assets"))
        {
            manager->currentDir.erase(manager->currentDir.begin() + 1, manager->currentDir.end());
            needRefreshList = true;
        }

        // Draw each folder in currentDir, ignore Assets
        for (size_t i = 1; i < manager->currentDir.size(); ++i)
        {
            ImGui::SameLine();
            ImGui::Text(">"); // separator
            ImGui::SameLine();

            if (ImGui::Button(manager->currentDir[i].c_str()))
            {
                // Go up to this folder
                manager->currentDir.erase(manager->currentDir.begin() + i + 1, manager->currentDir.end());
                needRefreshList = true;
            }
        }
    }
}

