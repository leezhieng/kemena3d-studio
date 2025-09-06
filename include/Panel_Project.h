#ifndef PANEL_PROJECT_H
#define PANEL_PROJECT_H

#include "kemena/kemena.h"

#include "manager.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace kemena;

class Manager;

class PanelProject
{
	private:
		Manager* manager;

		ImTextureRef iconUp;
		ImTextureRef iconAdd;
		ImTextureRef iconMag;

		ImTextureRef iconFolder;
		ImTextureRef iconText;
		ImTextureRef iconImage;
		ImTextureRef iconScript;
		ImTextureRef iconAudio;
		ImTextureRef iconVideo;
		ImTextureRef iconModel;
		ImTextureRef iconPrefab;
		ImTextureRef iconWorld;
		ImTextureRef iconMaterial;
		ImTextureRef iconOther;

		char searchBuffer[128] = "";

		ImVec4 upTint = ImVec4(1, 1, 1, 1);
		ImVec4 addTint = ImVec4(1, 1, 1, 1);

		ImTextureRef iconList;
		ImTextureRef iconThumbnail;

		bool displayThumbnail = false;

		struct Node
		{
			std::string name;
			bool isSelected = false;
			std::vector<std::unique_ptr<Node>> children;

			Node(const std::string& n) : name(n) {}
		};

		Node root;

	public:
	    PanelProject();

		void init(Manager* setManager, kAssetManager* assetManager);
		void deselectAll(Node& root);
		void drawNode(Node& node, Node& root);
		void drawProjectPanel(Node& root, bool* opened, bool enabled);
		void draw(kGuiManager* gui, bool& opened, bool enabled);
		void refreshList();
};

#endif

