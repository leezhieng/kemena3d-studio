#ifndef PANEL_PROJECT_H
#define PANEL_PROJECT_H

#include "kemena/kemena.h"

#include "manager.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <filesystem>

using namespace kemena;
namespace fs = std::filesystem;

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

		bool displayThumbnail = true;

		struct Node
		{
			std::string name;
			bool isSelected = false;
			std::vector<std::unique_ptr<Node>> children;

			std::string uuid;
			ImTextureRef icon = nullptr;
			int type = 0; // 0 - Folder, 1 - File

			Node(const std::string& n, const std::string& g, ImTextureRef i = nullptr, int t = 0) : name(n), uuid(g), icon(i), type(t) {}
		};

		Node rootTree;
		Node rootThumbnail;
		bool needRefreshList = false;

	public:
	    PanelProject(Manager* setManager, kAssetManager* assetManager);

		void deselectAll(Node& root);

		void drawProjectPanel(Node& rootTree, Node& rootThumbnail, bool* opened, bool enabled);
		void draw(kGuiManager* gui, bool& opened, bool enabled);

		void refreshTreeList();
		void drawTreeNode(Node& node, Node& rootTree, int level = 0);
		void populateTree(Node& parent, const fs::path& path);

		void refreshThumbnailList();
		void drawThumbnailNode(const Node& currentDir);
		void drawBreadcrumb();
};

#endif

