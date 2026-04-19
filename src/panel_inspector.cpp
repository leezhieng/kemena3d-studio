#include "panel_inspector.h"
#include "panel_project.h"
#include "commands.h"

using namespace kemena;

PanelInspector::PanelInspector(kGuiManager *setGuiManager, Manager *setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

// ---------------------------------------------------------------------------
// Table helpers
// ---------------------------------------------------------------------------

static bool beginPropTable(kGuiManager *gui, const char *id)
{
    if (!gui->tableStart(id, 2, ImGuiTableFlags_SizingStretchProp))
        return false;
    gui->tableSetupColumn("L", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    gui->tableSetupColumn("V", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

static void propLabel(kGuiManager *gui, const char *label)
{
    gui->tableNextRow();
    gui->tableSetColumnIndex(0);
    gui->alignTextToFramePadding();
    gui->text(label);
    if (gui->isItemHovered() && gui->calcTextSize(label).x > gui->getColumnWidth())
    {
        gui->beginTooltip();
        gui->textUnformatted(label);
        gui->endTooltip();
    }
    gui->tableSetColumnIndex(1);
    gui->setNextItemWidth(-FLT_MIN);
}

// ---------------------------------------------------------------------------
// Transform section
// ---------------------------------------------------------------------------
static void drawTransformSection(kGuiManager *gui, kObject *obj, Manager *mgr)
{
    if (!gui->collapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "TfmTable"))
        return;

    // --- Position ---
    {
        static kVec3 s_posBefore;

        kVec3 pos = obj->getPosition();
        kVec3 posPreEdit = pos;
        float p[3] = {pos.x, pos.y, pos.z};

        propLabel(gui, "Position");
        if (gui->dragFloat3("##Pos", p, 0.1f))
            obj->setPosition(kVec3(p[0], p[1], p[2]));
        if (gui->isItemActivated())
            s_posBefore = posPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = obj->getPosition();
            kVec3 before = s_posBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setPosition(before); },
                [cap, after]()
                { cap->setPosition(after); }));
        }
    }

    // --- Rotation ---
    {
        static kVec3 s_rotBefore;

        kVec3 euler = obj->getRotationEuler();
        kVec3 eulerPreEdit = euler;
        float r[3] = {euler.x, euler.y, euler.z};
        for (int i = 0; i < 3; i++)
            if (r[i] == 0.0f)
                r[i] = 0.0f;

        propLabel(gui, "Rotation");
        if (gui->dragFloat3("##Rot", r, 0.5f))
            obj->setRotation(kQuat(glm::radians(kVec3(r[0], r[1], r[2]))));
        if (gui->isItemActivated())
            s_rotBefore = eulerPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = obj->getRotationEuler();
            kVec3 before = s_rotBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setRotation(kQuat(glm::radians(before))); },
                [cap, after]()
                { cap->setRotation(kQuat(glm::radians(after))); }));
        }
    }

    // --- Scale ---
    {
        static kVec3 s_scaleBefore;

        kVec3 scl = obj->getScale();
        kVec3 sclPreEdit = scl;
        float s[3] = {scl.x, scl.y, scl.z};

        propLabel(gui, "Scale");
        if (gui->dragFloat3("##Scl", s, 0.01f, 0.001f, 10000.0f))
            obj->setScale(kVec3(s[0], s[1], s[2]));
        if (gui->isItemActivated())
            s_scaleBefore = sclPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = obj->getScale();
            kVec3 before = s_scaleBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setScale(before); },
                [cap, after]()
                { cap->setScale(after); }));
        }
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Mesh section
// ---------------------------------------------------------------------------
static void drawMeshSection(kGuiManager *gui, kMesh *mesh, Manager *mgr)
{
    if (!gui->collapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "MeshTable"))
        return;

    propLabel(gui, "File");
    gui->beginDisabled(true);
    char fileBuf[256];
    strncpy_s(fileBuf, sizeof(fileBuf), mesh->getFileName().c_str(), _TRUNCATE);
    ImGui::InputText("##MeshFile", fileBuf, sizeof(fileBuf));
    gui->endDisabled();

    propLabel(gui, "Cast Shadow");
    bool castShadow = mesh->getCastShadow();
    bool prevCastShadow = castShadow;
    if (gui->checkbox("##CastShadow", &castShadow))
    {
        mesh->setCastShadow(castShadow);
        kMesh *cap = mesh;
        bool after = castShadow;
        bool before = prevCastShadow;
        mgr->undoRedo.push(std::make_unique<PropertyCommand>(
            [cap, before]()
            { cap->setCastShadow(before); },
            [cap, after]()
            { cap->setCastShadow(after); }));
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Camera section
// ---------------------------------------------------------------------------
static void drawCameraSection(kGuiManager *gui, kCamera *cam, Manager *mgr)
{
    if (!gui->collapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "CamTable"))
        return;

    // FOV
    {
        static float s_fovBefore = 0.0f;
        float fov = cam->getFOV();
        float fovPreEdit = fov;
        propLabel(gui, "FOV");
        if (gui->dragFloat("##FOV", &fov, 0.5f, 1.0f, 179.0f, "%.1f deg"))
            cam->setFOV(fov);
        if (gui->isItemActivated())
            s_fovBefore = fovPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after = cam->getFOV();
            float before = s_fovBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setFOV(before); },
                [cap, after]()
                { cap->setFOV(after); }));
        }
    }

    // Near Clip
    {
        static float s_nearBefore = 0.0f;
        float nearClip = cam->getNearClip();
        float nearPreEdit = nearClip;
        propLabel(gui, "Near Clip");
        if (gui->dragFloat("##Near", &nearClip, 0.01f, 0.001f, 10000.0f, "%.3f"))
            cam->setNearClip(nearClip);
        if (gui->isItemActivated())
            s_nearBefore = nearPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after = cam->getNearClip();
            float before = s_nearBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setNearClip(before); },
                [cap, after]()
                { cap->setNearClip(after); }));
        }
    }

    // Far Clip
    {
        static float s_farBefore = 0.0f;
        float farClip = cam->getFarClip();
        float farPreEdit = farClip;
        propLabel(gui, "Far Clip");
        if (gui->dragFloat("##Far", &farClip, 1.0f, 0.1f, 100000.0f, "%.1f"))
            cam->setFarClip(farClip);
        if (gui->isItemActivated())
            s_farBefore = farPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after = cam->getFarClip();
            float before = s_farBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setFarClip(before); },
                [cap, after]()
                { cap->setFarClip(after); }));
        }
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Light section
// ---------------------------------------------------------------------------
static void drawLightSection(kGuiManager *gui, kLight *light, Manager *mgr)
{
    kLightType lt = light->getLightType();

    const char *header = "Light";
    if (lt == LIGHT_TYPE_SUN)
        header = "Light (Sun)";
    else if (lt == LIGHT_TYPE_POINT)
        header = "Light (Point)";
    else if (lt == LIGHT_TYPE_SPOT)
        header = "Light (Spot)";

    if (!gui->collapsingHeader(header, ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "LightTable"))
        return;

    // Power
    {
        static float s_pwrBefore = 0.0f;
        float pwr = light->getPower();
        float pwrPreEdit = pwr;
        propLabel(gui, "Power");
        if (gui->dragFloat("##Power", &pwr, 0.1f, 0.0f, 10000.0f))
            light->setPower(pwr);
        if (gui->isItemActivated())
            s_pwrBefore = pwrPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after = light->getPower();
            float before = s_pwrBefore;
            kLight *cap = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setPower(before); },
                [cap, after]()
                { cap->setPower(after); }));
        }
    }

    // Diffuse
    {
        static kVec3 s_diffBefore;
        kVec3 diff = light->getDiffuseColor();
        kVec3 diffPreEdit = diff;
        float diffF[3] = {diff.r, diff.g, diff.b};
        propLabel(gui, "Diffuse");
        if (gui->colorEdit3("##Diffuse", diffF))
            light->setDiffuseColor(kVec3(diffF[0], diffF[1], diffF[2]));
        if (gui->isItemActivated())
            s_diffBefore = diffPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = light->getDiffuseColor();
            kVec3 before = s_diffBefore;
            kLight *cap = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setDiffuseColor(before); },
                [cap, after]()
                { cap->setDiffuseColor(after); }));
        }
    }

    // Specular
    {
        static kVec3 s_specBefore;
        kVec3 spec = light->getSpecularColor();
        kVec3 specPreEdit = spec;
        float specF[3] = {spec.r, spec.g, spec.b};
        propLabel(gui, "Specular");
        if (gui->colorEdit3("##Specular", specF))
            light->setSpecularColor(kVec3(specF[0], specF[1], specF[2]));
        if (gui->isItemActivated())
            s_specBefore = specPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = light->getSpecularColor();
            kVec3 before = s_specBefore;
            kLight *cap = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setSpecularColor(before); },
                [cap, after]()
                { cap->setSpecularColor(after); }));
        }
    }

    if (lt == LIGHT_TYPE_POINT || lt == LIGHT_TYPE_SPOT)
    {
        // Constant
        {
            static float s_constBefore = 0.0f;
            float c = light->getConstant();
            float cPreEdit = c;
            propLabel(gui, "Constant");
            if (gui->dragFloat("##Constant", &c, 0.01f, 0.0f, 10.0f))
                light->setConstant(c);
            if (gui->isItemActivated())
                s_constBefore = cPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after = light->getConstant();
                float before = s_constBefore;
                kLight *cap = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]()
                    { cap->setConstant(before); },
                    [cap, after]()
                    { cap->setConstant(after); }));
            }
        }
        // Linear
        {
            static float s_linBefore = 0.0f;
            float l = light->getLinear();
            float lPreEdit = l;
            propLabel(gui, "Linear");
            if (gui->dragFloat("##Linear", &l, 0.01f, 0.0f, 10.0f))
                light->setLinear(l);
            if (gui->isItemActivated())
                s_linBefore = lPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after = light->getLinear();
                float before = s_linBefore;
                kLight *cap = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]()
                    { cap->setLinear(before); },
                    [cap, after]()
                    { cap->setLinear(after); }));
            }
        }
        // Quadratic
        {
            static float s_quadBefore = 0.0f;
            float q = light->getQuadratic();
            float qPreEdit = q;
            propLabel(gui, "Quadratic");
            if (gui->dragFloat("##Quadratic", &q, 0.01f, 0.0f, 10.0f))
                light->setQuadratic(q);
            if (gui->isItemActivated())
                s_quadBefore = qPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after = light->getQuadratic();
                float before = s_quadBefore;
                kLight *cap = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]()
                    { cap->setQuadratic(before); },
                    [cap, after]()
                    { cap->setQuadratic(after); }));
            }
        }
    }

    if (lt == LIGHT_TYPE_SPOT)
    {
        // Inner cone
        {
            static float s_innerBefore = 0.0f;
            float inner = glm::degrees(glm::acos(light->getCutOff()));
            float innerPreEdit = inner;
            propLabel(gui, "Inner Cone");
            if (gui->dragFloat("##InnerCone", &inner, 0.5f, 0.0f, 89.0f, "%.1f deg"))
                light->setCutOff(glm::cos(glm::radians(inner)));
            if (gui->isItemActivated())
                s_innerBefore = innerPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after = glm::degrees(glm::acos(light->getCutOff()));
                float before = s_innerBefore;
                kLight *cap = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]()
                    { cap->setCutOff(glm::cos(glm::radians(before))); },
                    [cap, after]()
                    { cap->setCutOff(glm::cos(glm::radians(after))); }));
            }
        }
        // Outer cone
        {
            static float s_outerBefore = 0.0f;
            float outer = glm::degrees(glm::acos(light->getOuterCutOff()));
            float outerPreEdit = outer;
            propLabel(gui, "Outer Cone");
            if (gui->dragFloat("##OuterCone", &outer, 0.5f, 0.0f, 89.0f, "%.1f deg"))
                light->setOuterCutOff(glm::cos(glm::radians(outer)));
            if (gui->isItemActivated())
                s_outerBefore = outerPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after = glm::degrees(glm::acos(light->getOuterCutOff()));
                float before = s_outerBefore;
                kLight *cap = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]()
                    { cap->setOuterCutOff(glm::cos(glm::radians(before))); },
                    [cap, after]()
                    { cap->setOuterCutOff(glm::cos(glm::radians(after))); }));
            }
        }
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Scene section
// ---------------------------------------------------------------------------
static void drawSceneSection(kGuiManager *gui, kScene *scene, Manager *mgr)
{
    if (!gui->collapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "SceneTable"))
        return;

    // Ambient color
    {
        static kVec3 s_ambBefore;
        kVec3 amb = scene->getAmbientLightColor();
        float ambF[3] = {amb.r, amb.g, amb.b};
        propLabel(gui, "Ambient");
        if (gui->colorEdit3("##SceneAmbient", ambF))
            scene->setAmbientLightColor(kVec3(ambF[0], ambF[1], ambF[2]));
        if (gui->isItemActivated())
            s_ambBefore = amb;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after = scene->getAmbientLightColor();
            kVec3 before = s_ambBefore;
            kScene *cap = scene;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setAmbientLightColor(before); },
                [cap, after]()
                { cap->setAmbientLightColor(after); }));
        }
    }

    // Skybox ambient toggle
    {
        bool enabled = scene->getSkyboxAmbientEnabled();
        propLabel(gui, "Sky Ambient");
        if (gui->checkbox("##SkyAmbient", &enabled))
        {
            bool before = !enabled;
            bool after = enabled;
            kScene *cap = scene;
            scene->setSkyboxAmbientEnabled(enabled);
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setSkyboxAmbientEnabled(before); },
                [cap, after]()
                { cap->setSkyboxAmbientEnabled(after); }));
        }
    }

    // Skybox ambient strength (only when enabled)
    if (scene->getSkyboxAmbientEnabled())
    {
        static float s_skyStrBefore = 0.0f;
        float str = scene->getSkyboxAmbientStrength();
        float strPreEdit = str;
        propLabel(gui, "Sky Strength");
        if (gui->dragFloat("##SkyStrength", &str, 0.01f, 0.0f, 5.0f))
            scene->setSkyboxAmbientStrength(str);
        if (gui->isItemActivated())
            s_skyStrBefore = strPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after = scene->getSkyboxAmbientStrength();
            float before = s_skyStrBefore;
            kScene *cap = scene;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]()
                { cap->setSkyboxAmbientStrength(before); },
                [cap, after]()
                { cap->setSkyboxAmbientStrength(after); }));
        }
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Project asset section
// ---------------------------------------------------------------------------

static void drawAssetThumbnail(kGuiManager *gui, const PanelProject::SelectedProjectAsset &asset)
{
    if (asset.thumbnail != nullptr)
    {
        float avail = gui->getContentRegionAvail().x;
        float imgSize = std::min(avail, 128.0f);
        gui->setCursorPosX(gui->getCursorPosX() + (avail - imgSize) * 0.5f);
        ImGui::Image(asset.thumbnail, ImVec2(imgSize, imgSize));
        gui->spacing();
    }
}

static void saveMetaJson(const fs::path &metaPath, const nlohmann::json &j)
{
    std::ofstream f(metaPath);
    if (f.is_open())
        f << j.dump(4);
}

static nlohmann::json loadMetaJson(const fs::path &metaPath)
{
    if (metaPath.empty())
        return nlohmann::json::object();
    try
    {
        if (!fs::exists(metaPath))
            return nlohmann::json::object();
        std::ifstream f(metaPath);
        if (!f.is_open())
            return nlohmann::json::object();
        return nlohmann::json::parse(f);
    }
    catch (...)
    {
        return nlohmann::json::object();
    }
}

// Mesh compression options
static const char *kMeshCompItems[] = {"None", "Low", "Medium", "High"};
// Tangent options
static const char *kTangentItems[] = {"Import", "Generate"};
// Animation compression options
static const char *kAnimCompItems[] = {"Off", "Keyframe Reduction", "Optimal"};

static void loadMeshSettings(const fs::path &metaPath,
                             float &scaleFactor, int &meshCompression, bool &generateCollider,
                             int &tangents, bool &generateLightmapUV, bool &importAnimation,
                             int &animCompression)
{
    auto j = loadMetaJson(metaPath);
    scaleFactor = j.value("scaleFactor", 1.0f);
    meshCompression = j.value("meshCompression", 0);
    generateCollider = j.value("generateCollider", false);
    tangents = j.value("tangents", 0);
    generateLightmapUV = j.value("generateLightmapUV", false);
    importAnimation = j.value("importAnimation", true);
    animCompression = j.value("animCompression", 0);
}

static void drawMeshImportSettings(kGuiManager *gui, const PanelProject::SelectedProjectAsset &asset, Manager *mgr)
{
    static kString lastUuid;
    static float scaleFactor = 1.0f;
    static int meshCompression = 0;
    static bool generateCollider = false;
    static int tangents = 0;
    static bool generateLightmapUV = false;
    static bool importAnimation = true;
    static int animCompression = 0;
    static bool dirty = false;

    if (asset.uuid != lastUuid)
    {
        lastUuid = asset.uuid;
        dirty = false;
        loadMeshSettings(asset.metaPath, scaleFactor, meshCompression, generateCollider, tangents, generateLightmapUV, importAnimation, animCompression);
    }

    if (!gui->collapsingHeader("Import Settings", ImGuiTreeNodeFlags_DefaultOpen))
        return;
    if (!beginPropTable(gui, "MeshImportTable"))
        return;

    propLabel(gui, "Scale Factor");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::DragFloat("##ScaleFactor", &scaleFactor, 0.001f, 0.0001f, 100.0f, "%.4f"))
        dirty = true;

    propLabel(gui, "Mesh Compression");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##MeshComp", &meshCompression, kMeshCompItems, IM_ARRAYSIZE(kMeshCompItems)))
        dirty = true;

    propLabel(gui, "Generate Collider");
    if (ImGui::Checkbox("##GenCollider", &generateCollider))
        dirty = true;

    propLabel(gui, "Tangents");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##Tangents", &tangents, kTangentItems, IM_ARRAYSIZE(kTangentItems)))
        dirty = true;

    propLabel(gui, "Lightmap UVs");
    if (ImGui::Checkbox("##LightmapUV", &generateLightmapUV))
        dirty = true;

    propLabel(gui, "Import Animation");
    if (ImGui::Checkbox("##ImportAnim", &importAnimation))
        dirty = true;

    propLabel(gui, "Anim Compression");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##AnimComp", &animCompression, kAnimCompItems, IM_ARRAYSIZE(kAnimCompItems)))
        dirty = true;

    propLabel(gui, "Material");
    if (ImGui::Button("Extract##Mat", ImVec2(-FLT_MIN, 0)))
    {
        // TODO: extract embedded materials from the mesh asset
    }

    propLabel(gui, "Texture");
    if (ImGui::Button("Extract##Tex", ImVec2(-FLT_MIN, 0)))
    {
        // TODO: extract embedded textures from the mesh asset
    }

    gui->tableEnd();
    gui->spacing();

    bool wasDisabled = !dirty;
    if (wasDisabled)
        gui->beginDisabled(true);
    float btnW = (gui->getContentRegionAvail().x - 4.0f) * 0.5f;
    if (ImGui::Button("Apply##Mesh", ImVec2(btnW, 0)) && !asset.metaPath.empty())
    {
        auto j = loadMetaJson(asset.metaPath);
        j["scaleFactor"] = scaleFactor;
        j["meshCompression"] = meshCompression;
        j["generateCollider"] = generateCollider;
        j["tangents"] = tangents;
        j["generateLightmapUV"] = generateLightmapUV;
        j["importAnimation"] = importAnimation;
        j["animCompression"] = animCompression;
        saveMetaJson(asset.metaPath, j);
        dirty = false;
    }
    gui->sameLine(0, 4.0f);
    if (ImGui::Button("Revert##Mesh", ImVec2(btnW, 0)))
    {
        loadMeshSettings(asset.metaPath, scaleFactor, meshCompression, generateCollider,
                         tangents, generateLightmapUV, importAnimation, animCompression);
        dirty = false;
    }
    if (wasDisabled)
        gui->endDisabled();
}

// Image type options
static const char *kImgTypeItems[] = {"Texture", "GUI", "Sprite"};
// Max size options
static const char *kMaxSizeItems[] = {"32", "64", "128", "256", "512", "1024", "2048", "4096"};
// Compression options
static const char *kImgCompItems[] = {"None", "Low Quality", "Normal Quality", "High Quality"};
// Alpha source options
static const char *kAlphaSrcItems[] = {"None", "Input Alpha", "From Grayscale"};
// Wrap mode options
static const char *kWrapModeItems[] = {"Repeat", "Clamp", "Mirror"};
// Filter mode options
static const char *kFilterModeItems[] = {"Point", "Bilinear", "Trilinear"};

static void loadImageSettings(const fs::path &metaPath,
                              int &imageType, int &maxSizeIndex, int &compression,
                              int &alphaSource, bool &sRGB, bool &generateMipmap, int &wrapMode, int &filterMode)
{
    auto j = loadMetaJson(metaPath);
    imageType = j.value("imageType", 0);
    maxSizeIndex = j.value("maxSizeIndex", 5);
    compression = j.value("compression", 2);
    alphaSource = j.value("alphaSource", 0);
    sRGB = j.value("sRGB", true);
    generateMipmap = j.value("generateMipmap", true);
    wrapMode = j.value("wrapMode", 0);
    filterMode = j.value("filterMode", 1);
}

static void drawImageImportSettings(kGuiManager *gui, const PanelProject::SelectedProjectAsset &asset, Manager *mgr)
{
    static kString lastUuid;
    static int imageType = 0;
    static int maxSizeIndex = 5;
    static int compression = 2;
    static int alphaSource = 0;
    static bool sRGB = true;
    static bool generateMipmap = true;
    static int wrapMode = 0;
    static int filterMode = 1;
    static bool dirty = false;

    if (asset.uuid != lastUuid)
    {
        lastUuid = asset.uuid;
        dirty = false;
        loadImageSettings(asset.metaPath, imageType, maxSizeIndex, compression,
                          alphaSource, sRGB, generateMipmap, wrapMode, filterMode);
    }

    if (!gui->collapsingHeader("Import Settings", ImGuiTreeNodeFlags_DefaultOpen))
        return;
    if (!beginPropTable(gui, "ImageImportTable"))
        return;

    propLabel(gui, "Image Type");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##ImgType", &imageType, kImgTypeItems, IM_ARRAYSIZE(kImgTypeItems)))
        dirty = true;

    propLabel(gui, "Max Size");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##MaxSize", &maxSizeIndex, kMaxSizeItems, IM_ARRAYSIZE(kMaxSizeItems)))
        dirty = true;

    propLabel(gui, "Compression");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##ImgComp", &compression, kImgCompItems, IM_ARRAYSIZE(kImgCompItems)))
        dirty = true;

    propLabel(gui, "Alpha Source");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##AlphaSrc", &alphaSource, kAlphaSrcItems, IM_ARRAYSIZE(kAlphaSrcItems)))
        dirty = true;

    propLabel(gui, "sRGB");
    if (ImGui::Checkbox("##sRGB", &sRGB))
        dirty = true;

    propLabel(gui, "Generate Mipmap");
    if (ImGui::Checkbox("##GenMipmap", &generateMipmap))
        dirty = true;

    propLabel(gui, "Wrap Mode");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##WrapMode", &wrapMode, kWrapModeItems, IM_ARRAYSIZE(kWrapModeItems)))
        dirty = true;

    propLabel(gui, "Filter Mode");
    gui->setNextItemWidth(-FLT_MIN);
    if (ImGui::Combo("##FilterMode", &filterMode, kFilterModeItems, IM_ARRAYSIZE(kFilterModeItems)))
        dirty = true;

    gui->tableEnd();
    gui->spacing();

    bool wasDisabled = !dirty;
    if (wasDisabled)
        gui->beginDisabled(true);
    float btnW = (gui->getContentRegionAvail().x - 4.0f) * 0.5f;
    if (ImGui::Button("Apply##Image", ImVec2(btnW, 0)) && !asset.metaPath.empty())
    {
        auto j = loadMetaJson(asset.metaPath);
        j["imageType"] = imageType;
        j["maxSizeIndex"] = maxSizeIndex;
        j["compression"] = compression;
        j["alphaSource"] = alphaSource;
        j["sRGB"] = sRGB;
        j["generateMipmap"] = generateMipmap;
        j["wrapMode"] = wrapMode;
        j["filterMode"] = filterMode;
        saveMetaJson(asset.metaPath, j);
        dirty = false;
    }
    gui->sameLine(0, 4.0f);
    if (ImGui::Button("Revert##Image", ImVec2(btnW, 0)))
    {
        loadImageSettings(asset.metaPath, imageType, maxSizeIndex, compression,
                          alphaSource, sRGB, generateMipmap, wrapMode, filterMode);
        dirty = false;
    }
    if (wasDisabled)
        gui->endDisabled();
}

// ---------------------------------------------------------------------------
// Main draw
// ---------------------------------------------------------------------------
void PanelInspector::draw(bool &opened)
{
    if (!opened)
        return;

    if (!manager->projectOpened)
        gui->beginDisabled(true);

    gui->windowStart("Inspector", &opened);

    // Project panel selection takes priority when scene selection is empty
    if (manager->panelProject != nullptr && manager->selectedObjects.empty() && manager->selectedScene == nullptr)
    {
        auto asset = manager->panelProject->getProjectSelection();
        if (asset.count > 1)
        {
            gui->spacing();
            kString text = std::to_string(asset.count) + " items selected";
            float tw = gui->calcTextSize(text).x;
            gui->setCursorPosX(gui->getCursorPosX() + (gui->getContentRegionAvail().x - tw) * 0.5f);
            gui->textDisabled(text);
            gui->windowEnd();
            if (!manager->projectOpened)
                gui->endDisabled();
            return;
        }
        if (asset.count == 1)
        {
            if (asset.isFolder)
            {
                gui->textDisabled("[Folder]");
                gui->sameLine();
                gui->textUnformatted(asset.name.c_str());
            }
            else
            {
                gui->textDisabled(("[" + asset.fileType + "]").c_str());
                gui->sameLine();
                gui->textUnformatted(asset.name.c_str());
            }
            gui->spacing();
            gui->separator();
            gui->spacing();
            drawAssetThumbnail(gui, asset);
            if (!asset.isFolder)
            {
                if (asset.fileType == "mesh")
                    drawMeshImportSettings(gui, asset, manager);
                else if (asset.fileType == "image")
                    drawImageImportSettings(gui, asset, manager);
            }
            gui->windowEnd();
            if (!manager->projectOpened)
                gui->endDisabled();
            return;
        }
    }

    if (manager->selectedScene != nullptr)
    {
        kScene *scene = manager->selectedScene;
        gui->textDisabled("[Scene]");
        gui->sameLine();
        {
            char nameBuf[256];
            strncpy_s(nameBuf, sizeof(nameBuf), scene->getName().c_str(), _TRUNCATE);
            gui->setNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##SceneName", nameBuf, sizeof(nameBuf),
                                 ImGuiInputTextFlags_EnterReturnsTrue))
            {
                kString before = scene->getName();
                kString after = kString(nameBuf);
                if (before != after)
                {
                    scene->setName(after);
                    kScene *cap = scene;
                    manager->undoRedo.push(std::make_unique<PropertyCommand>(
                        [cap, before]()
                        { cap->setName(before); },
                        [cap, after]()
                        { cap->setName(after); }));
                }
            }
        }
        gui->spacing();
        gui->separator();
        gui->spacing();
        drawSceneSection(gui, scene, manager);
    }
    else
    {

        size_t selCount = manager->selectedObjects.size();

        if (selCount == 0)
        {
            gui->spacing();
            const char *text = "Nothing is selected";
            float tw = gui->calcTextSize(text).x;
            gui->setCursorPosX(gui->getCursorPosX() + (gui->getContentRegionAvail().x - tw) * 0.5f);
            gui->textDisabled(text);
        }
        else if (selCount > 1)
        {
            gui->spacing();
            kString text = std::to_string(selCount) + " objects selected";
            float tw = gui->calcTextSize(text).x;
            gui->setCursorPosX(gui->getCursorPosX() + (gui->getContentRegionAvail().x - tw) * 0.5f);
            gui->textDisabled(text);
        }
        else
        {
            kObject *obj = manager->selectedObject;

            if (obj == nullptr)
            {
                gui->textDisabled("(object not found)");
            }
            else
            {
                kNodeType type = obj->getType();

                const char *typeLabel = "Object";
                if (type == NODE_TYPE_MESH)
                    typeLabel = "Mesh";
                else if (type == NODE_TYPE_LIGHT)
                    typeLabel = "Light";
                else if (type == NODE_TYPE_CAMERA)
                    typeLabel = "Camera";

                gui->textDisabled(kString("[") + typeLabel + "]");
                gui->sameLine();

                {
                    char nameBuf[256];
                    strncpy_s(nameBuf, sizeof(nameBuf), obj->getName().c_str(), _TRUNCATE);
                    gui->setNextItemWidth(-FLT_MIN);
                    if (ImGui::InputText("##ObjName", nameBuf, sizeof(nameBuf),
                                         ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        kString before = obj->getName();
                        kString after = kString(nameBuf);
                        if (before != after)
                        {
                            obj->setName(after);
                            kObject *cap = obj;
                            manager->undoRedo.push(std::make_unique<PropertyCommand>(
                                [cap, before]()
                                { cap->setName(before); },
                                [cap, after]()
                                { cap->setName(after); }));
                        }
                    }
                }

                {
                    bool active = obj->getActive();
                    bool prevActive = active;
                    if (gui->checkbox("Active", &active))
                    {
                        obj->setActive(active);
                        kObject *cap = obj;
                        bool after = active;
                        bool before = prevActive;
                        manager->undoRedo.push(std::make_unique<PropertyCommand>(
                            [cap, before]()
                            { cap->setActive(before); },
                            [cap, after]()
                            { cap->setActive(after); }));
                    }

                    gui->sameLine();
                    {
                        bool isStatic = obj->getStatic();
                        bool prevIsStatic = isStatic;
                        if (gui->checkbox("Static", &isStatic))
                        {
                            obj->setStatic(isStatic);
                            manager->getRenderer()->setOctreeDirty();
                            kObject *cap = obj;
                            bool after = isStatic;
                            bool before = prevIsStatic;
                            manager->undoRedo.push(std::make_unique<PropertyCommand>(
                                [cap, before]()
                                { cap->setStatic(before); },
                                [cap, after]()
                                { cap->setStatic(after); }));
                        }
                    }
                }

                gui->spacing();
                gui->separator();
                gui->spacing();

                drawTransformSection(gui, obj, manager);
                gui->spacing();

                if (type == NODE_TYPE_MESH)
                    drawMeshSection(gui, static_cast<kMesh *>(obj), manager);
                else if (type == NODE_TYPE_LIGHT)
                    drawLightSection(gui, static_cast<kLight *>(obj), manager);
                else if (type == NODE_TYPE_CAMERA)
                    drawCameraSection(gui, static_cast<kCamera *>(obj), manager);
            }
        }

    } // end selectedScene else

    gui->windowEnd();

    if (!manager->projectOpened)
        gui->endDisabled();
}
