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

	private:
		// Scene object type icons
		ImTextureRef iconObjMesh   = nullptr;
		ImTextureRef iconObjLight  = nullptr;
		ImTextureRef iconObjCamera = nullptr;
		ImTextureRef iconObjScene  = nullptr;

		// File type icons
		ImTextureRef iconFileModel    = nullptr;
		ImTextureRef iconFileImage    = nullptr;
		ImTextureRef iconFileFolder   = nullptr;
		ImTextureRef iconFileMaterial = nullptr;
		ImTextureRef iconFilePrefab   = nullptr;
		ImTextureRef iconFileAudio    = nullptr;
		ImTextureRef iconFileVideo    = nullptr;
		ImTextureRef iconFileScript   = nullptr;
		ImTextureRef iconFileText     = nullptr;
		ImTextureRef iconFileWorld    = nullptr;
		ImTextureRef iconFileOther    = nullptr;

		ImTextureRef getFileTypeIcon(const kString& fileType) const;
};

#endif

