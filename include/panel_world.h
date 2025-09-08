#ifndef PANEL_WORLD_H
#define PANEL_WORLD_H

#include <algorithm>

#include "kemena/kemena.h"

using namespace kemena;

class PanelWorld
{
	public:
		bool enabled = false;
		bool hovered = false;
		bool focused = false;

		int width = 0;
		int height = 0;
		float aspectRatio = 0;

		void draw(kGuiManager* gui, bool& isOpened, bool isEnabled, kRenderer* renderer, kCamera* editorCamera);
};

#endif
