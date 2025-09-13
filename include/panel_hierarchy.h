#ifndef PANEL_HIERARCHY_H
#define PANEL_HIERARCHY_H

#include "kemena/kemena.h"
#include "kemena/kworld.h"

#include "manager.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace kemena;

class Manager;

class PanelHierarchy
{
	private:
		ImTextureRef iconAdd;
		ImTextureRef iconMag;

        ImTextureRef iconWorld;
        ImTextureRef iconScene;
        ImTextureRef iconMesh;
        ImTextureRef iconEmpty;
        ImTextureRef iconLight;
        ImTextureRef iconCamera;
        ImTextureRef iconPrefab;

		char searchBuffer[128] = "";

		ImVec4 addTint = ImVec4(1, 1, 1, 1);

		kWorld* world;
		Manager* manager;

		struct Node
		{
			std::string name;
			bool isSelected = false;
			std::vector<std::unique_ptr<Node>> children;

			std::string uuid;
			ImTextureRef icon = nullptr;
			std::string type = ""; // world, scene, mesh, etc.

			Node(const std::string& n, const std::string& g, ImTextureRef i = nullptr, const std::string& t = "")
            : name(n), uuid(g), icon(i), type(t) {}
		};

		Node root;

	public:
	    PanelHierarchy(Manager* setManager, kAssetManager* assetManager, kWorld* setWorld);
		void deselectAll(Node& root);
		void drawNode(Node& node, Node& root, int level);
		void drawHierarchyPanel(Node& root, bool* opened, bool enabled);
		void draw(kGuiManager* gui, bool& opened, bool enabled);
		void refreshList();
};

#endif

