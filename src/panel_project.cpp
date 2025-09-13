#include "panel_project.h"

using namespace kemena;

PanelProject::PanelProject(Manager* setManager, kAssetManager* assetManager)
	: rootTree("Assets", "asset", nullptr, 0), rootThumbnail("Assets", "asset", nullptr, 0)
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

	rootTree = Node("Asset", "asset", (ImTextureID)(intptr_t)tex_folder->getTextureID(), 0);
	rootThumbnail = Node("Asset", "asset", (ImTextureID)(intptr_t)tex_folder->getTextureID(), 0);
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
			if (ImGui::ImageButton("UpButton",
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

			// Add tooltip
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Go to parent directory");
            }
		}

		// Put next item on the same line
		ImGui::SameLine();

		// Add button
		{
			if (ImGui::ImageButton("AddButton",
								   iconAdd,
								   ImVec2(16, 16),
								   ImVec2(0, 0), ImVec2(1,1), // UVs
								   ImVec4(0, 0, 0, 0), // Background color
								   addTint)) // Tint color
			{
			}
			addTint = ImGui::IsItemActive() ? ImVec4(1, 1, 1, 0.5f) : ImVec4(1, 1, 1, 1);

			// Add tooltip
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Import assets to project");
            }
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
			ImGui::InputTextWithHint("##SearchProject", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
			ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, 8.0f);

			// List or Thumbnail button
			{
				if (displayThumbnail)
				{
					if (ImGui::ImageButton("ListButton",
										   iconList,
										   ImVec2(16, 16),
										   ImVec2(0,0), ImVec2(1,1), // UVs
										   ImVec4(0, 0, 0, 0), // Background color
										   ImVec4(1, 1, 1, 1))) // Tint color
					{
						displayThumbnail = false;
					}

					// Add tooltip
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Switch to list view");
                    }
				}
				else
				{
					if (ImGui::ImageButton("ThumbnailButton",
										   iconThumbnail,
										   ImVec2(16, 16),
										   ImVec2(0,0), ImVec2(1,1), // UVs
										   ImVec4(0, 0, 0, 0), // Background color
										   ImVec4(1, 1, 1, 1))) // Tint color
					{
						displayThumbnail = true;
					}

					// Add tooltip
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Switch to thumbnail view");
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

	fs::path fullPath = manager->projectPath;
	fullPath /= "Assets"; // Tree view always show the full project structure

	if (!fs::exists(fullPath) || !fs::is_directory(fullPath))
	{
		std::cerr << "Path does not exist or is not a directory: " << fullPath << "\n";
		return;
	}

	// Root node setup
	std::string folderName = fullPath.filename().string();
	rootTree.name = folderName;
	rootTree.uuid = "asset";
	rootTree.icon = iconFolder;
	rootTree.type = 0;
	rootTree.isSelected = false;
	rootTree.children.clear();

	// Fill recursively
	populateTree(rootTree, fullPath);
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

	bool nodeOpen = ImGui::TreeNodeEx(node.uuid.c_str(), flags, "%s", node.name.c_str());

	// Detect double-click on this tree node
	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		// Handle double-click
		std::cout << "Double-clicked: " << node.uuid.c_str() << " ,Level:" << level << std::endl;

		// Folder
		if (node.type == 0)
		{
			// Toggle open/close
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

void PanelProject::populateTree(Node& parent, const fs::path& fullPath)
{
	if (fs::is_empty(fullPath))
		return;

    fs::path assetsPath = manager->projectPath / "Assets";

	// --- First: Directories ---
	for (const auto& entry : fs::directory_iterator(fullPath))
	{
		if (entry.is_directory())
		{
			auto child = std::make_unique<Node>(
							 entry.path().filename().string(),
							 "Folder_" + entry.path().filename().string(),
							 iconFolder,
							 0
						 );

			// Recursively fill subfolder
			populateTree(*child, entry.path());

			parent.children.emplace_back(std::move(child));
		}
	}

	// --- Second: Files ---
	for (const auto& entry : fs::directory_iterator(fullPath))
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

            std::string relativePath = fs::relative(entry.path(), assetsPath).generic_string();

            std::string uuid = manager->uuidMap[relativePath];
            //std::cout << relativePath << " -> " << uuid << std::endl;

			parent.children.emplace_back(std::make_unique<Node>(
											 entry.path().filename().string(),
											 uuid,
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
		fs::path fullPath = manager->projectPath;
		fs::path assetsPath = manager->projectPath / "Assets";

		if (!manager->currentDir.empty())
		{
			for (const auto& dir : manager->currentDir)
            {
				fullPath /= dir;  // appends with correct separator for the platform
            }
		}

		if (!fs::exists(fullPath) || !fs::is_directory(fullPath))
		{
			std::cerr << "Path does not exist or is not a directory: " << fullPath << "\n";
			return;
		}

		if (fs::is_directory(fullPath))
		{
			std::string folderName = fullPath.filename().string();
			//std::cout << "Last folder: " << folderName << "\n";
			//std::cout << "Last folder: " << fullPath << "\n";

			// Check whether uuid already exist for this folder
			// Generate a uuid if haven't, then save to the list

			// Reset root safely
			rootThumbnail.name = folderName;
			rootThumbnail.uuid = "Assets";
			rootThumbnail.icon = iconFolder;
			rootThumbnail.type = 0;
			rootThumbnail.isSelected = false;
			rootThumbnail.children.clear();
		}

		// Loop through all the folders and files in fullPath
		if (!fs::exists(fullPath))
		{
			std::cerr << "Path does not exist: " << fullPath << "\n";
			return;
		}

		// Do directory first
		{
			if (!fs::is_empty(fullPath))
			{
				// Do for folders
				{
					for (const auto& entry : fs::directory_iterator(fullPath))
					{
						if (entry.is_directory())
						{
							rootThumbnail.children.emplace_back(std::make_unique<Node>(
																	entry.path().filename().string(),
																	"Folder_" + entry.path().filename().string(),
																	iconFolder,
																	0
																));
						}
					}
				}

				// Do for files
				{
					for (const auto& entry : fs::directory_iterator(fullPath))
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

                            std::string relativePath = fs::relative(entry.path(), assetsPath).generic_string();
                            std::string uuid = manager->uuidMap[relativePath];
                            //std::cout << relativePath << " -> " << uuid << std::endl;

							rootThumbnail.children.emplace_back(std::make_unique<Node>(
																	entry.path().filename().string(),
																	uuid,
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
	if (manager->projectOpened)
	{
		float thumbSize   = 48.0f;   // icon size
		float cellPadding = 12.0f;   // spacing between columns
		float cellWidth   = thumbSize + cellPadding;

		float panelWidth  = ImGui::GetContentRegionAvail().x;
		int columns = (int)(panelWidth / cellWidth);
		if (columns < 1) columns = 1;

		if (currentDir.children.size() > 0)
		{
			ImGui::Columns(columns, nullptr, false);

			for (auto& child : currentDir.children)
			{
				ImGui::BeginGroup();
				ImGui::PushID(child.get());

				float columnWidth = ImGui::GetColumnWidth();
				float cellHeight  = thumbSize + ImGui::GetTextLineHeight() + 4.0f; // total cell height

				// Full-cell selectable
				bool selected = child->isSelected;
				if (ImGui::Selectable("##thumb", selected, 0, ImVec2(columnWidth, cellHeight)))
				{
					child->isSelected = !child->isSelected;
				}

				// Handle double-click
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
				    if (child->type == 0)
                    {
                        // Folder
                        manager->openFolder(child->name);
                        needRefreshList = true;
                        ImGui::Columns(1); // reset columns before returning
                        ImGui::PopID();
                        ImGui::EndGroup();
                        return;
                    }
                    else
                    {
                        // File
                        std::cout << "Double-clicked: " << child->uuid.c_str() << std::endl;
                    }
				}

				std::string displayName = fitTextWithEllipsisUtf8(child->name, columnWidth - 4.0f);

				// Show tooltip only if text was truncated
				if (displayName != child->name && ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::TextUnformatted(child->name.c_str()); // full name
					ImGui::EndTooltip();
				}

				// Draw icon centered in selectable
				ImVec2 cellPos  = ImGui::GetItemRectMin();
				float iconX = cellPos.x + (columnWidth - thumbSize) * 0.5f;
				float iconY = cellPos.y;
				ImGui::SetCursorScreenPos(ImVec2(iconX, iconY));
				ImGui::Image(child->icon, ImVec2(thumbSize, thumbSize));

				// Draw text centered under icon
				float textWidth = ImGui::CalcTextSize(displayName.c_str()).x;
				float textX = cellPos.x + (columnWidth - textWidth) * 0.5f;
				float textY = iconY + thumbSize + 2.0f; // padding below icon
				ImGui::SetCursorScreenPos(ImVec2(textX, textY));
				ImGui::TextUnformatted(displayName.c_str());

				ImGui::PopID();
				ImGui::EndGroup();
				ImGui::NextColumn();
			}

			ImGui::Columns(1); // reset
		}
		else
		{
			std::string text = "Empty folder";
			float textWidth = ImGui::CalcTextSize(text.c_str()).x;
			float columnWidth = ImGui::GetColumnWidth();
			float textX = ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f; // center horizontally
			ImGui::SetCursorPosX(textX);

			ImGui::Text(text.c_str());
		}
	}
	else
	{
		std::string text = "No project found";
		float textWidth = ImGui::CalcTextSize(text.c_str()).x;
		float columnWidth = ImGui::GetColumnWidth();
		float textX = ImGui::GetCursorPosX() + (columnWidth - textWidth) * 0.5f; // center horizontally
		ImGui::SetCursorPosX(textX);

		ImGui::Text(text.c_str());
	}
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
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 3.0f);
			ImGui::Text(">"); // separator
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4.0f);

			if (ImGui::Button(manager->currentDir[i].c_str()))
			{
				// Go up to this folder
				manager->currentDir.erase(manager->currentDir.begin() + i + 1, manager->currentDir.end());
				needRefreshList = true;
			}
		}

		ImGui::Dummy(ImVec2(0.0f, 4.0f));
	}
}

