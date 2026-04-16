#ifndef PANEL_INSPECTOR_H
#define PANEL_INSPECTOR_H

#include "kemena/kemena.h"
#include <kemena/kmesh.h>
#include <kemena/klight.h>
#include <kemena/kcamera.h>
#include <glm/gtc/matrix_transform.hpp>

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

