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

		char searchBuffer[128] = "";

		ImVec4 addTint = ImVec4(1, 1, 1, 1);

		kWorld* world;
		Manager* manager;

		struct Node
		{
			std::string name;
			bool isSelected = false;
			std::vector<std::unique_ptr<Node>> children;

			std::string guid;
			ImTextureID icon;

			Node(const std::string& n) : name(n) {}
		};

		Node root;

	public:
	    PanelHierarchy(Manager* setManager, kAssetManager* assetManager, kWorld* setWorld);
		void deselectAll(Node& root);
		void drawNode(Node& node, Node& root);
		void drawHierarchyPanel(Node& root, bool* opened, bool enabled);
		void draw(kGuiManager* gui, bool& opened, bool enabled);
		void refreshList();
};

#endif

