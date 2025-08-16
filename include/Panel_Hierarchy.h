#ifndef PANEL_HIERARCHY_H
#define PANEL_HIERARCHY_H

#include "kemena/kemena.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace kemena;

namespace hierarchy
{
	struct Node
	{
		std::string name;
		bool isSelected = false;
		std::vector<std::unique_ptr<Node>> children;

		Node(const std::string& n) : name(n) {}
	};

	bool opened = true;
	Node root("World");

	void deselectAll(Node& root)
	{
		root.isSelected = false;
		for (auto& child : root.children)
		{
			deselectAll(*child);
		}
	}

	void drawNode(Node& node, Node& root)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (node.isSelected) flags |= ImGuiTreeNodeFlags_Selected;
		if (node.children.empty()) flags |= ImGuiTreeNodeFlags_Leaf;

		bool nodeOpen = ImGui::TreeNodeEx(node.name.c_str(), flags);

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
			for (auto& child : node.children)
			{
				drawNode(*child, root);
			}
			ImGui::TreePop();
		}
	}

	void drawHierarchyPanel(Node& root, bool* pOpen)
	{
		if (ImGui::Begin("Hierarchy", pOpen))
		{
			drawNode(root, root);
		}
		ImGui::End();
	}

	void init(kGuiManager* gui)
	{
		root.children.push_back(std::make_unique<Node>("Cube"));
		root.children.push_back(std::make_unique<Node>("Cube2"));
		root.children.push_back(std::make_unique<Node>("Cube3"));
		root.children.push_back(std::make_unique<Node>("Cube4"));
	}

	void draw(kGuiManager* gui)
	{
	    if (opened)
            drawHierarchyPanel(root, &opened);
	}
}

#endif

