#include "panel_project.h"

using namespace kemena;

PanelProject::PanelProject(kGuiManager* setGuiManager, Manager* setManager, kAssetManager* assetManager)
	: rootTree("Assets", "asset", nullptr, 0), rootThumbnail("Assets", "asset", nullptr, 0)
{
    gui = setGuiManager;
	manager = setManager;
	manager->panelProject = this;
	this->assetManager = assetManager;

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

ImTextureRef PanelProject::getThumbnailIcon(const kString& uuid, ImTextureRef defaultIcon)
{
	if (uuid.empty()) return defaultIcon;

	auto it = thumbnailCache.find(uuid);
	if (it != thumbnailCache.end())
		return it->second.second;

	fs::path thumbPath = manager->projectPath / "Library" / "Thumbnails" / (uuid + ".png");
	if (!fs::exists(thumbPath)) return defaultIcon;

	try
	{
		kTexture2D* tex = assetManager->loadTexture2D(
			thumbPath.string(), uuid, kTextureFormat::TEX_FORMAT_RGBA, false);
		if (tex && tex->getTextureID() != 0)
		{
			GLuint glId = (GLuint)tex->getTextureID();
			ImTextureRef ref = (ImTextureRef)(intptr_t)glId;
			thumbnailCache[uuid] = { glId, ref };
			return ref;
		}
	}
	catch (...) {}

	return defaultIcon;
}

void PanelProject::deselectAll(Node& root)
{
	root.isSelected = false;
	for (auto& child : root.children)
		deselectAll(*child);
}

void PanelProject::clearSelection()
{
	deselectAll(rootTree);
	deselectAll(rootThumbnail);
}

void PanelProject::drawProjectPanel(Node& rootTree, Node& rootThumbnail, bool* opened)
{
	gui->beginDisabled(!manager->projectOpened);

	gui->windowStart("Project", opened);
	{
		gui->pushStyleColor(ImGuiCol_Button, kVec4(0, 0, 0, 0));
		gui->pushStyleColor(ImGuiCol_ButtonHovered, kVec4(0, 0, 0, 0));
		gui->pushStyleColor(ImGuiCol_ButtonActive, kVec4(0, 0, 0, 0));

		gui->pushStyleVar(ImGuiStyleVar_ItemSpacing, kVec2(2, 0));

		// Up button
		{
			if (ImGui::ImageButton("UpButton", iconUp, ImVec2(16, 16)))
			{
				manager->closeFolder();
				needRefreshList = true;
			}
			upTint = gui->isItemActive() ? ImVec4(1,1,1,0.5f) : ImVec4(1,1,1,1);

			if (gui->isItemHovered())
				gui->setItemTooltip("Go to parent directory");
		}

		gui->sameLine();

		// Add button
		{
			ImGui::ImageButton("AddButton", iconAdd, ImVec2(16, 16));
			addTint = gui->isItemActive() ? ImVec4(1, 1, 1, 0.5f) : ImVec4(1, 1, 1, 1);

			if (gui->isItemHovered())
				gui->setItemTooltip("Import assets to project");
		}

		gui->popStyleVar();
		gui->popStyleColor(3);

		gui->sameLine();

		gui->pushStyleVar(ImGuiStyleVar_ItemSpacing, kVec2(0, 0));

		// Search bar
		gui->pushStyleVar(ImGuiStyleVar_FramePadding, kVec2(4, (22 - gui->getFontSize()) * 0.5f));
		gui->pushItemWidth(-FLT_MIN);

		gui->popStyleVar();

		gui->groupStart();
		{
			float iconSize = gui->getFontSize() * 0.8f;
			kVec2 cursor = gui->getCursorScreenPos();
			float buttonWidth = 18.0f;
			float searchWidth = gui->getContentRegionAvail().x - buttonWidth - 15;

			ImGui::GetWindowDrawList()->AddImage(
				iconMag,
				ImVec2(cursor.x + 4, cursor.y + (gui->getFrameHeight() - iconSize) * 0.5f),
				ImVec2(cursor.x + 4 + iconSize, cursor.y + (gui->getFrameHeight() + iconSize) * 0.5f)
			);

			gui->pushStyleVar(ImGuiStyleVar_FramePadding, kVec2(iconSize + 8, 3));
			gui->setNextItemWidth(searchWidth);
			ImGui::InputTextWithHint("##SearchProject", "Search...", searchBuffer, IM_ARRAYSIZE(searchBuffer));
			gui->popStyleVar();

			gui->sameLine(0.0f, 8.0f);

			// List or Thumbnail button
			{
				if (displayThumbnail)
				{
					if (ImGui::ImageButton("ListButton", iconList, ImVec2(16, 16)))
						displayThumbnail = false;

					if (gui->isItemHovered())
						gui->setItemTooltip("Switch to list view");
				}
				else
				{
					if (ImGui::ImageButton("ThumbnailButton", iconThumbnail, ImVec2(16, 16)))
						displayThumbnail = true;

					if (gui->isItemHovered())
						gui->setItemTooltip("Switch to thumbnail view");
				}
			}
		}
		gui->groupEnd();

		gui->popItemWidth();
		gui->popStyleVar();

		gui->spacing();
		gui->spacing();

		float availableHeight = gui->getContentRegionAvail().y - 4;

		if (displayThumbnail)
		{
			gui->childStart("ProjectThumbnail", kVec2(0, availableHeight), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
			{
				drawBreadcrumb();
				drawThumbnailNode(rootThumbnail);
			}
			gui->childEnd();
		}
		else
		{
			gui->childStart("ProjectTree", kVec2(0, availableHeight), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
			{
				drawTreeNode(rootTree, rootTree);
			}
			gui->childEnd();
		}

		gui->spacing();
	}
	gui->windowEnd();

	gui->endDisabled();
}

void PanelProject::draw(bool& opened)
{
	if (manager->projectOpened)
	{
		if (needRefreshList)
		{
			clearThumbnailCache();
			refreshTreeList();
			refreshThumbnailList();

			needRefreshList = false;
		}

		drawProjectPanel(rootTree, rootThumbnail, &opened);
	}
}

void PanelProject::clearThumbnailCache()
{
	for (auto& [uuid, entry] : thumbnailCache)
	{
		if (entry.first) glDeleteTextures(1, &entry.first);
	}
	thumbnailCache.clear();
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
	kString folderName = fullPath.filename().string();
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

	ImGui::Image(node.icon, ImVec2(16, 16));
	gui->sameLine();

	bool nodeOpen = gui->treeStartEx(node.uuid, node.name, flags);

	if (gui->isItemHovered() && gui->isMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		std::cout << "Double-clicked: " << node.uuid.c_str() << " ,Level:" << level << std::endl;
	}

	if (gui->isItemClicked())
	{
		if (!gui->isKeyShift())
			deselectAll(rootTree);
		node.isSelected = !node.isSelected || gui->isKeyShift();
		manager->selectedObjects.clear();
		manager->selectedObject = nullptr;
	}

	if (nodeOpen)
	{
		level++;

		for (auto& child : node.children)
			drawTreeNode(*child, rootTree, level);

		gui->treePop();
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

            kString relativePath = fs::relative(entry.path(), assetsPath).generic_string();

            kString uuid = manager->uuidMap[relativePath];
            //std::cout << relativePath << " -> " << uuid << std::endl;

			icon = getThumbnailIcon(uuid, icon);

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

		// Navigate up if the current directory no longer exists
		while ((!fs::exists(fullPath) || !fs::is_directory(fullPath)) && manager->currentDir.size() > 1)
		{
			manager->currentDir.pop_back();
			fullPath = manager->projectPath;
			for (const auto& dir : manager->currentDir)
				fullPath /= dir;
		}

		if (!fs::exists(fullPath) || !fs::is_directory(fullPath))
		{
			std::cerr << "Path does not exist or is not a directory: " << fullPath << "\n";
			return;
		}

		if (fs::is_directory(fullPath))
		{
			kString folderName = fullPath.filename().string();
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

                            kString relativePath = fs::relative(entry.path(), assetsPath).generic_string();
                            kString uuid = manager->uuidMap[relativePath];
                            //std::cout << relativePath << " -> " << uuid << std::endl;

							icon = getThumbnailIcon(uuid, icon);

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

		float panelWidth  = gui->getContentRegionAvail().x;
		int columns = (int)(panelWidth / cellWidth);
		if (columns < 1) columns = 1;

		if (currentDir.children.size() > 0)
		{
			gui->columnsStart(columns, "", false);

			for (auto& child : currentDir.children)
			{
				gui->groupStart();
				gui->pushId(child.get());

				float columnWidth = gui->getColumnWidth();
				float cellHeight  = thumbSize + gui->getTextLineHeight() + 4.0f;

				// Full-cell selectable
				bool selected = child->isSelected;
				if (gui->selectable("##thumb", selected, 0, kVec2(columnWidth, cellHeight)))
				{
					child->isSelected = !child->isSelected;
					manager->selectedObjects.clear();
					manager->selectedObject = nullptr;
				}

				// Handle double-click
				if (gui->isItemHovered() && gui->isMouseDoubleClicked(ImGuiMouseButton_Left))
				{
				    if (child->type == 0)
                    {
                        manager->openFolder(child->name);
                        needRefreshList = true;
                        gui->columnsEnd();
                        gui->popId();
                        gui->groupEnd();
                        return;
                    }
                    else
                    {
                        std::cout << "Double-clicked: " << child->uuid.c_str() << std::endl;
                    }
				}

				kString displayName = fitTextWithEllipsisUtf8(gui, child->name, columnWidth - 4.0f);

				if (displayName != child->name && gui->isItemHovered())
				{
					gui->beginTooltip();
					gui->textUnformatted(child->name);
					gui->endTooltip();
				}

				kVec2 cellPos  = gui->getItemRectMin();
				float iconX = cellPos.x + (columnWidth - thumbSize) * 0.5f;
				float iconY = cellPos.y;
				gui->setCursorScreenPos(kVec2(iconX, iconY));
				ImGui::Image(child->icon, ImVec2(thumbSize, thumbSize));

				float textWidth = gui->calcTextSize(displayName).x;
				float textX = cellPos.x + (columnWidth - textWidth) * 0.5f;
				float textY = iconY + thumbSize + 2.0f;
				gui->setCursorScreenPos(kVec2(textX, textY));
				gui->textUnformatted(displayName);

				gui->popId();
				gui->groupEnd();
				gui->nextColumn();
			}

			gui->columnsEnd();
		}
		else
		{
			kString text = "Empty folder";
			float textWidth = gui->calcTextSize(text).x;
			float columnWidth = gui->getColumnWidth();
			float textX = gui->getCursorPosX() + (columnWidth - textWidth) * 0.5f;
			gui->setCursorPosX(textX);
			gui->text(text);
		}
	}
	else
	{
		kString text = "No project found";
		float textWidth = gui->calcTextSize(text).x;
		float columnWidth = gui->getColumnWidth();
		float textX = gui->getCursorPosX() + (columnWidth - textWidth) * 0.5f;
		gui->setCursorPosX(textX);
		gui->text(text);
	}
}

void PanelProject::drawBreadcrumb()
{
	if (manager->currentDir.size() > 0)
	{
		// Start from project root
		fs::path basePath = manager->projectPath;

		// Draw Assets folder
		if (gui->button("Assets"))
		{
			manager->currentDir.erase(manager->currentDir.begin() + 1, manager->currentDir.end());
			needRefreshList = true;
		}

		for (size_t i = 1; i < manager->currentDir.size(); ++i)
		{
			gui->sameLine();
			gui->setCursorPosX(gui->getCursorPosX() - 3.0f);
			gui->text(">");
			gui->sameLine();
			gui->setCursorPosX(gui->getCursorPosX() - 4.0f);

			if (gui->button(manager->currentDir[i]))
			{
				manager->currentDir.erase(manager->currentDir.begin() + i + 1, manager->currentDir.end());
				needRefreshList = true;
			}
		}

		gui->dummy(kVec2(0.0f, 4.0f));
	}
}

