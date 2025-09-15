#ifndef PANEL_INSPECTOR_H
#define PANEL_INSPECTOR_H

#include "kemena/kemena.h"

#include "manager.h"

using namespace kemena;

class PanelInspector
{
	public:
		PanelInspector(kGuiManager* setGuiManager, Manager* setManager);
		void draw(bool& opened);

		Manager* manager;
		kGuiManager* gui;
};

#endif

