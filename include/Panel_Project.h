#ifndef PANEL_PROJECT_H
#define PANEL_PROJECT_H

#include "kemena/kemena.h"

#include <imgui.h>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

using namespace kemena;

namespace project
{
	struct Node
	{
		std::string name;
		bool isSelected = false;
		std::vector<std::unique_ptr<Node>> children;

		Node(const std::string& n) : name(n) {}
	};

	Node root("Assets");

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

	void drawProjectPanel(Node& root, bool* opened, bool* enabled)
	{
	    if (!enabled)
            ImGui::BeginDisabled(true);

		if (ImGui::Begin("Project", opened))
		{
			drawNode(root, root);
		}
		ImGui::End();

		if (!enabled)
            ImGui::EndDisabled();
	}

	void init(kGuiManager* gui)
	{
		/*root.children.push_back(std::make_unique<Node>("Mesh"));
		root.children.push_back(std::make_unique<Node>("Texture"));
		root.children.push_back(std::make_unique<Node>("Audio"));
		root.children.push_back(std::make_unique<Node>("Script"));*/
	}

	void draw(kGuiManager* gui, bool& opened, bool enabled)
	{
	    if (opened)
            drawProjectPanel(root, &opened, &enabled);
	}
}

#endif

