#ifndef PANEL_INSPECTOR_H
#define PANEL_INSPECTOR_H

#include "kemena/kemena.h"

using namespace kemena;

class PanelInspector
{
	public:
		PanelInspector();
		void draw(kGuiManager* gui, bool& opened, bool enabled, std::vector<kObject*> selectedObjects = std::vector<kObject*>());
};

#endif

