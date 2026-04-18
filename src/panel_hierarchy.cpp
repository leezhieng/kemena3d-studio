#include "panel_hierarchy.h"
#include "commands.h"

using namespace kemena;

PanelHierarchy::PanelHierarchy(kGuiManager *setGuiManager, Manager *setManager, kAssetManager *assetManager, kWorld *setWorld)
	: root("World", "world", 0, "world")
{
	gui = setGuiManager;
	manager = setManager;
	manager->panelHierarchy = this;

	kTexture2D *tex_add = assetManager->loadTexture2DFromResource("ICON_ADD_ROUND_BUTTON", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconAdd = tex_add->getTextureID();

	kTexture2D *tex_mag = assetManager->loadTexture2DFromResource("ICON_MAGNIFIER_LABEL", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMag = tex_mag->getTextureID();

	// Object type icons
	kTexture2D *tex_world = assetManager->loadTexture2DFromResource("ICON_OBJECT_WORLD", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconWorld = tex_world->getTextureID();

	kTexture2D *tex_scene = assetManager->loadTexture2DFromResource("ICON_OBJECT_SCENE", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconScene = tex_scene->getTextureID();

	kTexture2D *tex_mesh = assetManager->loadTexture2DFromResource("ICON_OBJECT_MESH", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconMesh = tex_mesh->getTextureID();

	kTexture2D *tex_empty = assetManager->loadTexture2DFromResource("ICON_OBJECT_EMPTY", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconEmpty = tex_empty->getTextureID();

	kTexture2D *tex_light = assetManager->loadTexture2DFromResource("ICON_OBJECT_LIGHT", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconLight = tex_light->getTextureID();

	kTexture2D *tex_camera = assetManager->loadTexture2DFromResource("ICON_OBJECT_CAMERA", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconCamera = tex_camera->getTextureID();

	kTexture2D *tex_prefab = assetManager->loadTexture2DFromResource("ICON_OBJECT_PREFAB", "icon", kTextureFormat::TEX_FORMAT_RGBA);
	iconPrefab = tex_prefab->getTextureID();

	world = setWorld;

	root = Node("World", "world", iconWorld, "world");
}

void PanelHierarchy::deselectAll(Node &root)
{
	root.isSelected = false;
	for (auto &child : root.children)
	{
		deselectAll(*child);
	}
}

void PanelHierarchy::drawNode(Node &node, Node &root, int level)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	if (node.isSelected)
		flags |= ImGuiTreeNodeFlags_Selected;
	if (node.children.empty())
		flags |= ImGuiTreeNodeFlags_Leaf;

	// Only expand world and scene by default
	if (level <= 1)
	{
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	// Draw the icon
	gui->image(node.icon, kVec2(16, 16));

	gui->sameLine();

	bool nodeOpen = gui->treeStartEx(node.uuid, node.name, flags);

	// Item clicked
	if (gui->isItemClicked())
	{
		// Snapshot selection before change (for undo)
		auto selBefore    = manager->selectedObjects;
		auto selObjBefore = manager->selectedObject;

		if (!gui->isKeyShift())
		{
			deselectAll(root);
		}
		node.isSelected = !node.isSelected || gui->isKeyShift();

		if (gui->isKeyShift())
			manager->selectObject(node.uuid, false);
		else
			manager->selectObject(node.uuid, true);

		std::cout << "Object clicked: " << node.uuid.c_str() << " ,Level:" << level << std::endl;

		kObject *newSelObj = manager->selectedObject;

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
				newSelObj = manager->selectedObject;
			}
			else
			{
				std::cout << "NOT FOUND: " << node.uuid.c_str() << std::endl;
			}
		}

		// Push selection undo command
		auto selAfter    = manager->selectedObjects;
		auto selObjAfter = newSelObj;
		if (selBefore != selAfter || selObjBefore != selObjAfter)
		{
			manager->undoRedo.push(std::make_unique<SelectCommand>(
				manager,
				selBefore,    selObjBefore,
				selAfter,     selObjAfter));
		}
	}

	if (nodeOpen)
	{
		level++;

		for (auto &child : node.children)
		{
			drawNode(*child, root, level);
		}
		gui->treePop();
	}
}

void PanelHierarchy::drawHierarchyPanel(Node &root, bool *opened)
{
	gui->beginDisabled(!manager->projectOpened);

	gui->windowStart("Hierarchy", opened);
	{
		gui->pushStyleColor(ImGuiCol_Button, kVec4(0, 0, 0, 0));		// Background
		gui->pushStyleColor(ImGuiCol_ButtonHovered, kVec4(0, 0, 0, 0)); // Hover
		gui->pushStyleColor(ImGuiCol_ButtonActive, kVec4(0, 0, 0, 0));	// Pressed

		gui->pushStyleVar(ImGuiStyleVar_ItemSpacing, kVec2(2, 0)); // smaller gap (2px horizontal, 0 vertical)

		// Add button
		{
			if (gui->imageButton("AddButton", iconAdd, kVec2(16, 16), kVec2(0, 0), kVec2(1, 1), addTint))
			{
			}
			addTint = gui->isItemActive() ? kVec4(1, 1, 1, 0.5f) : kVec4(1, 1, 1, 1);
		}

		gui->popStyleVar(); // Restore spacing
		gui->popStyleColor(3);

		// Put search box on the same line
		gui->sameLine();

		// Search bar
		gui->pushStyleVar(ImGuiStyleVar_FramePadding, kVec2(4, (22 - gui->getFontSize()) * 0.5f));
		gui->pushItemWidth(-FLT_MIN);

		gui->groupStart();
		{
			float iconSize = gui->getFontSize() * 0.8f; // scale relative to text height
			kVec2 cursor = gui->getCursorScreenPos();

			// Draw the icon over the input box (aligned left-center)
			gui->drawListAddImage(
				iconMag,
				kVec2(cursor.x + 4, cursor.y + (gui->getFrameHeight() - iconSize) * 0.5f),
				kVec2(cursor.x + 4 + iconSize, cursor.y + (gui->getFrameHeight() + iconSize) * 0.5f)
			);

			gui->pushStyleVar(ImGuiStyleVar_FramePadding, kVec2(iconSize + 8, 3));

			// Input aligned with button height
			gui->setNextItemWidth(-FLT_MIN);
			gui->inputTextWithHint("##SearchHierarchy", "Search...", searchBuffer);
		}
		gui->groupEnd();

		gui->popItemWidth();
		gui->popStyleVar(2);

		gui->spacing();

		// Tree view
		{
			float availableHeight = gui->getContentRegionAvail().y - 4; // 4 px spacing

			gui->childStart("HierarchyTree", kVec2(0, availableHeight), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
			{
				if (manager->projectOpened)
				{
					drawNode(root, root, 0);
				}
				else
				{
					kString text = "No active world";
					float textWidth = gui->calcTextSize(text).x;
					float columnWidth = gui->getColumnWidth();
					float textX = gui->getCursorPosX() + (columnWidth - textWidth) * 0.5f; // center horizontally
					gui->setCursorPosX(textX);

					gui->text(text);
				}
			}
			gui->childEnd();
		}
	}
	gui->windowEnd();

	gui->endDisabled();
}

void PanelHierarchy::draw(bool &opened)
{
	if (opened)
		drawHierarchyPanel(root, &opened);
}

void PanelHierarchy::refreshList()
{
	manager->objectMap.clear();
	root.children.clear();

	if (world->getScenes().size() > 1)
	{
		for (size_t i = 1; i < world->getScenes().size(); ++i)
		{
			kScene *scene = world->getScenes().at(i);

			// Add scene to root
			auto &sceneNode = root.children.emplace_back(std::make_unique<Node>(scene->getName(), scene->getUuid(), iconScene, "scene"));

			// Loop through objects in the scene
			kObject *rootNode = scene->getRootNode();
			if (rootNode->getChildren().size() > 0)
			{
				for (size_t j = 0; j < rootNode->getChildren().size(); ++j)
				{
					kObject *childNode = rootNode->getChildren().at(j);
					GLuint icon = 0;
					kString type = "";

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
