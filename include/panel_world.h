#ifndef PANEL_WORLD_H
#define PANEL_WORLD_H

#include <algorithm>

#include "kemena/kemena.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "manager.h"
#include <ImGuizmo.h>

using namespace kemena;

class Manager;

class PanelWorld
{
	public:
		bool enabled = false;
		bool hovered = false;
		bool focused = false;

		int width = 0;
		int height = 0;
		float aspectRatio = 0;
		ImVec2 panelPos = ImVec2(0.f, 0.f); ///< Absolute screen-space top-left of the viewport content area.

		PanelWorld(kGuiManager* setGuiManager, Manager* setManager);
		void draw(bool& isOpened, kRenderer* renderer, kCamera* editorCamera);

		Manager* manager;
		kGuiManager* gui;
};

#endif
