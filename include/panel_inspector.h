#ifndef PANEL_INSPECTOR_H
#define PANEL_INSPECTOR_H

#include "kemena/kemena.h"
#include <kemena/kmesh.h>
#include <kemena/klight.h>
#include <kemena/kcamera.h>
#include <kemena/kscene.h>
#include <kemena/koffscreenrenderer.h>
#include <kemena/ktexture2d.h>
#include <glm/gtc/matrix_transform.hpp>

#include "manager.h"

using namespace kemena;

class PanelInspector
{
	public:
		PanelInspector(kGuiManager* setGuiManager, Manager* setManager);
		~PanelInspector();
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

		// -----------------------------------------------------------------------
		// Shader preview
		// -----------------------------------------------------------------------
		kOffscreenRenderer* previewRenderer = nullptr;
		kWorld*             previewWorld    = nullptr;  // standalone, not the editor world
		kScene*             previewScene    = nullptr;  // owned by previewWorld
		kCamera*            previewCamera   = nullptr;  // manually owned
		kLight*             previewLight    = nullptr;  // scene-owned sun light
		kMesh*              previewMesh     = nullptr;  // scene-owned sphere
		kShader*            previewShader   = nullptr;  // rebuilt on GLSL change
		kMaterial*          previewMat      = nullptr;  // rebuilt on GLSL change
		std::vector<kTexture2D*> previewDefaultTextures;  // white 1×1 placeholders per sampler

		std::string prevUuid;
		std::string prevGlsl;
		bool        prevValid = false;

		// Preview params
		float prevDiffuse[3]  = {1.0f, 1.0f, 1.0f};
		float prevSpecular[3] = {1.0f, 1.0f, 1.0f};
		float prevShininess   = 32.0f;
		float prevMetallic    = 0.0f;
		float prevRoughness   = 0.5f;

		// Light control
		bool  lightEnabled     = true;
		float lightYaw         = 45.0f;   // azimuth, degrees
		float lightPitch       = 60.0f;   // elevation, degrees
		bool  isDraggingLight  = false;

		void drawShaderPreview();
		void initPreviewScene();
		void updatePreviewLight();
		void loadPreviewParams(const std::string& uuid);
		void savePreviewParams(const std::string& uuid);
		void rebuildPreviewShader();
		void applyPreviewParams();

		// -----------------------------------------------------------------------
		// Model viewer
		// -----------------------------------------------------------------------
		kOffscreenRenderer*       modelViewRenderer    = nullptr;
		kWorld*                   modelViewWorld       = nullptr;
		kScene*                   modelViewScene       = nullptr;
		kCamera*                  modelViewCamera      = nullptr;
		kLight*                   modelViewLight       = nullptr;
		kMesh*                    modelViewMesh        = nullptr;  // owned by asset manager
		std::vector<kMaterial*>   modelViewDefaultMats;            // default mats we created

		std::string  modelViewUuid;
		bool         modelViewLightEnabled =  false;
		float        modelViewLightYaw     =  45.0f;
		float        modelViewLightPitch   =  60.0f;
		float        modelViewRotX         =  24.09f;  // orbit pitch — matches thumbnail dir normalize(0.5,0.5,1)
		float        modelViewRotY         =  26.57f;  // orbit yaw
		kVec3        modelViewCenter      =  kVec3(0.0f);
		float        modelViewCamDist     =  3.0f;
		bool         isDraggingMVLight    = false;
		bool         isDraggingMVModel    = false;

		void drawModelViewer(const PanelProject::SelectedProjectAsset& asset);
		void initModelViewScene();
		void updateModelViewLight();
		void frameModelViewCamera();
		void applyDefaultMaterial(kMesh* mesh, kShader* defaultShader);

		// -----------------------------------------------------------------------
		// Material inspector + viewer (shared live state)
		// -----------------------------------------------------------------------
		nlohmann::json  matInspJson  = nlohmann::json::object();
		std::string     matInspUuid;
		bool            matInspDirty   = false;
		int             matInspVersion = 0;   // incremented on every property change

		kOffscreenRenderer* matViewRenderer   = nullptr;
		kWorld*             matViewWorld      = nullptr;
		kScene*             matViewScene      = nullptr;
		kCamera*            matViewCamera     = nullptr;
		kLight*             matViewLight      = nullptr;
		kMesh*              matViewMesh       = nullptr;  // sphere, manually owned
		kMaterial*          matViewMat        = nullptr;  // manually owned
		int                 matViewVersion    = -1;       // version last rendered
		std::string         matViewUuid;

		float        matViewLightYaw     =  45.0f;
		float        matViewLightPitch   =  60.0f;
		bool         matViewLightEnabled = true;
		bool         isDraggingMatLight  = false;

		void drawMaterialViewer(const PanelProject::SelectedProjectAsset& asset);
		void drawMaterialInspector(const PanelProject::SelectedProjectAsset& asset);
		void initMatViewScene();
		void updateMatViewLight();
		void rebuildMatViewMaterial(const nlohmann::json& matJson);
};

#endif

