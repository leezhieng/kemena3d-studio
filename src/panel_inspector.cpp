#include "panel_inspector.h"
#include "commands.h"

using namespace kemena;

PanelInspector::PanelInspector(kGuiManager *setGuiManager, Manager *setManager)
{
    gui     = setGuiManager;
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

        kVec3 pos        = obj->getPosition();
        kVec3 posPreEdit = pos;
        float p[3]       = { pos.x, pos.y, pos.z };

        propLabel(gui, "Position");
        if (gui->dragFloat3("##Pos", p, 0.1f))
            obj->setPosition(kVec3(p[0], p[1], p[2]));
        if (gui->isItemActivated())
            s_posBefore = posPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after  = obj->getPosition();
            kVec3 before = s_posBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setPosition(before); },
                [cap, after]()  { cap->setPosition(after); }));
        }
    }

    // --- Rotation ---
    {
        static kVec3 s_rotBefore;

        kVec3 euler        = obj->getRotationEuler();
        kVec3 eulerPreEdit = euler;
        float r[3]         = { euler.x, euler.y, euler.z };

        propLabel(gui, "Rotation");
        if (gui->dragFloat3("##Rot", r, 0.5f))
            obj->setRotation(kQuat(glm::radians(kVec3(r[0], r[1], r[2]))));
        if (gui->isItemActivated())
            s_rotBefore = eulerPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after  = obj->getRotationEuler();
            kVec3 before = s_rotBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setRotation(kQuat(glm::radians(before))); },
                [cap, after]()  { cap->setRotation(kQuat(glm::radians(after))); }));
        }
    }

    // --- Scale ---
    {
        static kVec3 s_scaleBefore;

        kVec3 scl        = obj->getScale();
        kVec3 sclPreEdit = scl;
        float s[3]       = { scl.x, scl.y, scl.z };

        propLabel(gui, "Scale");
        if (gui->dragFloat3("##Scl", s, 0.01f, 0.001f, 10000.0f))
            obj->setScale(kVec3(s[0], s[1], s[2]));
        if (gui->isItemActivated())
            s_scaleBefore = sclPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3 after  = obj->getScale();
            kVec3 before = s_scaleBefore;
            kObject *cap = obj;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setScale(before); },
                [cap, after]()  { cap->setScale(after); }));
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
    bool castShadow     = mesh->getCastShadow();
    bool prevCastShadow = castShadow;
    if (gui->checkbox("##CastShadow", &castShadow))
    {
        mesh->setCastShadow(castShadow);
        kMesh *cap   = mesh;
        bool   after = castShadow;
        bool   before = prevCastShadow;
        mgr->undoRedo.push(std::make_unique<PropertyCommand>(
            [cap, before]() { cap->setCastShadow(before); },
            [cap, after]()  { cap->setCastShadow(after); }));
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
        float fov        = cam->getFOV();
        float fovPreEdit = fov;
        propLabel(gui, "FOV");
        if (gui->dragFloat("##FOV", &fov, 0.5f, 1.0f, 179.0f, "%.1f deg"))
            cam->setFOV(fov);
        if (gui->isItemActivated())
            s_fovBefore = fovPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after  = cam->getFOV();
            float before = s_fovBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setFOV(before); },
                [cap, after]()  { cap->setFOV(after); }));
        }
    }

    // Near Clip
    {
        static float s_nearBefore = 0.0f;
        float nearClip        = cam->getNearClip();
        float nearPreEdit     = nearClip;
        propLabel(gui, "Near Clip");
        if (gui->dragFloat("##Near", &nearClip, 0.01f, 0.001f, 10000.0f, "%.3f"))
            cam->setNearClip(nearClip);
        if (gui->isItemActivated())
            s_nearBefore = nearPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after  = cam->getNearClip();
            float before = s_nearBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setNearClip(before); },
                [cap, after]()  { cap->setNearClip(after); }));
        }
    }

    // Far Clip
    {
        static float s_farBefore = 0.0f;
        float farClip        = cam->getFarClip();
        float farPreEdit     = farClip;
        propLabel(gui, "Far Clip");
        if (gui->dragFloat("##Far", &farClip, 1.0f, 0.1f, 100000.0f, "%.1f"))
            cam->setFarClip(farClip);
        if (gui->isItemActivated())
            s_farBefore = farPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after  = cam->getFarClip();
            float before = s_farBefore;
            kCamera *cap = cam;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setFarClip(before); },
                [cap, after]()  { cap->setFarClip(after); }));
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
    if (lt == LIGHT_TYPE_SUN)        header = "Light (Sun)";
    else if (lt == LIGHT_TYPE_POINT) header = "Light (Point)";
    else if (lt == LIGHT_TYPE_SPOT)  header = "Light (Spot)";

    if (!gui->collapsingHeader(header, ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable(gui, "LightTable"))
        return;

    // Power
    {
        static float s_pwrBefore = 0.0f;
        float pwr        = light->getPower();
        float pwrPreEdit = pwr;
        propLabel(gui, "Power");
        if (gui->dragFloat("##Power", &pwr, 0.1f, 0.0f, 10000.0f))
            light->setPower(pwr);
        if (gui->isItemActivated())
            s_pwrBefore = pwrPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            float after  = light->getPower();
            float before = s_pwrBefore;
            kLight *cap  = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setPower(before); },
                [cap, after]()  { cap->setPower(after); }));
        }
    }

    // Ambient
    {
        static kVec3 s_ambBefore;
        kVec3 amb        = light->getAmbientColor();
        kVec3 ambPreEdit = amb;
        float ambF[3]    = { amb.r, amb.g, amb.b };
        propLabel(gui, "Ambient");
        if (gui->colorEdit3("##Ambient", ambF))
            light->setAmbientColor(kVec3(ambF[0], ambF[1], ambF[2]));
        if (gui->isItemActivated())
            s_ambBefore = ambPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3   after  = light->getAmbientColor();
            kVec3   before = s_ambBefore;
            kLight *cap    = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setAmbientColor(before); },
                [cap, after]()  { cap->setAmbientColor(after); }));
        }
    }

    // Diffuse
    {
        static kVec3 s_diffBefore;
        kVec3 diff        = light->getDiffuseColor();
        kVec3 diffPreEdit = diff;
        float diffF[3]    = { diff.r, diff.g, diff.b };
        propLabel(gui, "Diffuse");
        if (gui->colorEdit3("##Diffuse", diffF))
            light->setDiffuseColor(kVec3(diffF[0], diffF[1], diffF[2]));
        if (gui->isItemActivated())
            s_diffBefore = diffPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3   after  = light->getDiffuseColor();
            kVec3   before = s_diffBefore;
            kLight *cap    = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setDiffuseColor(before); },
                [cap, after]()  { cap->setDiffuseColor(after); }));
        }
    }

    // Specular
    {
        static kVec3 s_specBefore;
        kVec3 spec        = light->getSpecularColor();
        kVec3 specPreEdit = spec;
        float specF[3]    = { spec.r, spec.g, spec.b };
        propLabel(gui, "Specular");
        if (gui->colorEdit3("##Specular", specF))
            light->setSpecularColor(kVec3(specF[0], specF[1], specF[2]));
        if (gui->isItemActivated())
            s_specBefore = specPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3   after  = light->getSpecularColor();
            kVec3   before = s_specBefore;
            kLight *cap    = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setSpecularColor(before); },
                [cap, after]()  { cap->setSpecularColor(after); }));
        }
    }

    if (lt == LIGHT_TYPE_SPOT)
    {
        static kVec3 s_dirBefore;
        kVec3 dir        = light->getDirection();
        kVec3 dirPreEdit = dir;
        float dirF[3]    = { dir.x, dir.y, dir.z };
        propLabel(gui, "Direction");
        if (gui->dragFloat3("##Dir", dirF, 0.01f, -1.0f, 1.0f))
            light->setDirection(kVec3(dirF[0], dirF[1], dirF[2]));
        if (gui->isItemActivated())
            s_dirBefore = dirPreEdit;
        if (gui->isItemDeactivatedAfterEdit())
        {
            kVec3   after  = light->getDirection();
            kVec3   before = s_dirBefore;
            kLight *cap    = light;
            mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                [cap, before]() { cap->setDirection(before); },
                [cap, after]()  { cap->setDirection(after); }));
        }
    }

    if (lt == LIGHT_TYPE_POINT || lt == LIGHT_TYPE_SPOT)
    {
        // Constant
        {
            static float s_constBefore = 0.0f;
            float c        = light->getConstant();
            float cPreEdit = c;
            propLabel(gui, "Constant");
            if (gui->dragFloat("##Constant", &c, 0.01f, 0.0f, 10.0f))
                light->setConstant(c);
            if (gui->isItemActivated())
                s_constBefore = cPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after  = light->getConstant();
                float before = s_constBefore;
                kLight *cap  = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]() { cap->setConstant(before); },
                    [cap, after]()  { cap->setConstant(after); }));
            }
        }
        // Linear
        {
            static float s_linBefore = 0.0f;
            float l        = light->getLinear();
            float lPreEdit = l;
            propLabel(gui, "Linear");
            if (gui->dragFloat("##Linear", &l, 0.01f, 0.0f, 10.0f))
                light->setLinear(l);
            if (gui->isItemActivated())
                s_linBefore = lPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after  = light->getLinear();
                float before = s_linBefore;
                kLight *cap  = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]() { cap->setLinear(before); },
                    [cap, after]()  { cap->setLinear(after); }));
            }
        }
        // Quadratic
        {
            static float s_quadBefore = 0.0f;
            float q        = light->getQuadratic();
            float qPreEdit = q;
            propLabel(gui, "Quadratic");
            if (gui->dragFloat("##Quadratic", &q, 0.01f, 0.0f, 10.0f))
                light->setQuadratic(q);
            if (gui->isItemActivated())
                s_quadBefore = qPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after  = light->getQuadratic();
                float before = s_quadBefore;
                kLight *cap  = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]() { cap->setQuadratic(before); },
                    [cap, after]()  { cap->setQuadratic(after); }));
            }
        }
    }

    if (lt == LIGHT_TYPE_SPOT)
    {
        // Inner cone
        {
            static float s_innerBefore = 0.0f;
            float inner        = glm::degrees(glm::acos(light->getCutOff()));
            float innerPreEdit = inner;
            propLabel(gui, "Inner Cone");
            if (gui->dragFloat("##InnerCone", &inner, 0.5f, 0.0f, 89.0f, "%.1f deg"))
                light->setCutOff(glm::cos(glm::radians(inner)));
            if (gui->isItemActivated())
                s_innerBefore = innerPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after  = glm::degrees(glm::acos(light->getCutOff()));
                float before = s_innerBefore;
                kLight *cap  = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]() { cap->setCutOff(glm::cos(glm::radians(before))); },
                    [cap, after]()  { cap->setCutOff(glm::cos(glm::radians(after))); }));
            }
        }
        // Outer cone
        {
            static float s_outerBefore = 0.0f;
            float outer        = glm::degrees(glm::acos(light->getOuterCutOff()));
            float outerPreEdit = outer;
            propLabel(gui, "Outer Cone");
            if (gui->dragFloat("##OuterCone", &outer, 0.5f, 0.0f, 89.0f, "%.1f deg"))
                light->setOuterCutOff(glm::cos(glm::radians(outer)));
            if (gui->isItemActivated())
                s_outerBefore = outerPreEdit;
            if (gui->isItemDeactivatedAfterEdit())
            {
                float after  = glm::degrees(glm::acos(light->getOuterCutOff()));
                float before = s_outerBefore;
                kLight *cap  = light;
                mgr->undoRedo.push(std::make_unique<PropertyCommand>(
                    [cap, before]() { cap->setOuterCutOff(glm::cos(glm::radians(before))); },
                    [cap, after]()  { cap->setOuterCutOff(glm::cos(glm::radians(after))); }));
            }
        }
    }

    gui->tableEnd();
}

// ---------------------------------------------------------------------------
// Main draw
// ---------------------------------------------------------------------------
void PanelInspector::draw(bool &opened)
{
    if (!opened) return;

    if (!manager->projectOpened)
        gui->beginDisabled(true);

    gui->windowStart("Inspector", &opened);

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
            if (type == NODE_TYPE_MESH)        typeLabel = "Mesh";
            else if (type == NODE_TYPE_LIGHT)  typeLabel = "Light";
            else if (type == NODE_TYPE_CAMERA) typeLabel = "Camera";

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
                    kString after  = kString(nameBuf);
                    if (before != after)
                    {
                        obj->setName(after);
                        kObject *cap = obj;
                        manager->undoRedo.push(std::make_unique<PropertyCommand>(
                            [cap, before]() { cap->setName(before); },
                            [cap, after]()  { cap->setName(after); }));
                    }
                }
            }

            {
                bool active     = obj->getActive();
                bool prevActive = active;
                if (gui->checkbox("Active", &active))
                {
                    obj->setActive(active);
                    kObject *cap   = obj;
                    bool     after = active;
                    bool     before = prevActive;
                    manager->undoRedo.push(std::make_unique<PropertyCommand>(
                        [cap, before]() { cap->setActive(before); },
                        [cap, after]()  { cap->setActive(after); }));
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

    gui->windowEnd();

    if (!manager->projectOpened)
        gui->endDisabled();
}
