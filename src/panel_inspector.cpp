#include "panel_inspector.h"

using namespace kemena;

PanelInspector::PanelInspector(kGuiManager* setGuiManager, Manager* setManager)
{
    gui = setGuiManager;
    manager = setManager;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Begin a two-column label/value table.
static bool beginPropTable(const char *id)
{
    if (!ImGui::BeginTable(id, 2, ImGuiTableFlags_SizingStretchProp))
        return false;
    ImGui::TableSetupColumn("L", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    ImGui::TableSetupColumn("V", ImGuiTableColumnFlags_WidthStretch);
    return true;
}

// Start a row, write the label, move to the value column ready for a widget.
static void propLabel(const char *label)
{
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);
    ImGui::TableSetColumnIndex(1);
    ImGui::SetNextItemWidth(-FLT_MIN);
}

// ---------------------------------------------------------------------------
// Transform section — reads live from the object every frame so gizmo
// changes are reflected automatically.
// ---------------------------------------------------------------------------
static void drawTransformSection(kObject *obj)
{
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable("TfmTable"))
        return;

    kVec3 pos   = obj->getPosition();
    kVec3 euler = obj->getRotationEuler(); // degrees
    kVec3 scl   = obj->getScale();

    float p[3] = { pos.x,   pos.y,   pos.z   };
    float r[3] = { euler.x, euler.y, euler.z };
    float s[3] = { scl.x,   scl.y,   scl.z   };

    propLabel("Position");
    if (ImGui::DragFloat3("##Pos", p, 0.1f))
        obj->setPosition(kVec3(p[0], p[1], p[2]));

    propLabel("Rotation");
    if (ImGui::DragFloat3("##Rot", r, 0.5f))
        obj->setRotation(kQuat(glm::radians(kVec3(r[0], r[1], r[2]))));

    propLabel("Scale");
    if (ImGui::DragFloat3("##Scl", s, 0.01f, 0.001f, 10000.0f))
        obj->setScale(kVec3(s[0], s[1], s[2]));

    ImGui::EndTable();
}

// ---------------------------------------------------------------------------
// Mesh section
// ---------------------------------------------------------------------------
static void drawMeshSection(kMesh *mesh)
{
    if (!ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable("MeshTable"))
        return;

    // Source file — read-only info
    propLabel("File");
    ImGui::BeginDisabled(true);
    char fileBuf[256];
    strncpy(fileBuf, mesh->getFileName().c_str(), sizeof(fileBuf) - 1);
    fileBuf[sizeof(fileBuf) - 1] = '\0';
    ImGui::InputText("##MeshFile", fileBuf, sizeof(fileBuf));
    ImGui::EndDisabled();

    // Cast shadow toggle
    propLabel("Cast Shadow");
    bool castShadow = mesh->getCastShadow();
    if (ImGui::Checkbox("##CastShadow", &castShadow))
        mesh->setCastShadow(castShadow);

    ImGui::EndTable();
}

// ---------------------------------------------------------------------------
// Camera section
// ---------------------------------------------------------------------------
static void drawCameraSection(kCamera *cam)
{
    if (!ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable("CamTable"))
        return;

    propLabel("FOV");
    float fov = cam->getFOV();
    if (ImGui::DragFloat("##FOV", &fov, 0.5f, 1.0f, 179.0f, "%.1f deg"))
        cam->setFOV(fov);

    propLabel("Near Clip");
    float nearClip = cam->getNearClip();
    if (ImGui::DragFloat("##Near", &nearClip, 0.01f, 0.001f, 10000.0f, "%.3f"))
        cam->setNearClip(nearClip);

    propLabel("Far Clip");
    float farClip = cam->getFarClip();
    if (ImGui::DragFloat("##Far", &farClip, 1.0f, 0.1f, 100000.0f, "%.1f"))
        cam->setFarClip(farClip);

    ImGui::EndTable();
}

// ---------------------------------------------------------------------------
// Light section
// ---------------------------------------------------------------------------
static void drawLightSection(kLight *light)
{
    kLightType lt = light->getLightType();

    const char *header = "Light";
    if (lt == LIGHT_TYPE_SUN)   header = "Light (Sun)";
    else if (lt == LIGHT_TYPE_POINT) header = "Light (Point)";
    else if (lt == LIGHT_TYPE_SPOT)  header = "Light (Spot)";

    if (!ImGui::CollapsingHeader(header, ImGuiTreeNodeFlags_DefaultOpen))
        return;

    if (!beginPropTable("LightTable"))
        return;

    // Power
    propLabel("Power");
    float power = light->getPower();
    if (ImGui::DragFloat("##Power", &power, 0.1f, 0.0f, 10000.0f))
        light->setPower(power);

    // Ambient
    propLabel("Ambient");
    kVec3 amb = light->getAmbientColor();
    float ambF[3] = { amb.r, amb.g, amb.b };
    if (ImGui::ColorEdit3("##Ambient", ambF))
        light->setAmbientColor(kVec3(ambF[0], ambF[1], ambF[2]));

    // Diffuse
    propLabel("Diffuse");
    kVec3 diff = light->getDiffuseColor();
    float diffF[3] = { diff.r, diff.g, diff.b };
    if (ImGui::ColorEdit3("##Diffuse", diffF))
        light->setDiffuseColor(kVec3(diffF[0], diffF[1], diffF[2]));

    // Specular
    propLabel("Specular");
    kVec3 spec = light->getSpecularColor();
    float specF[3] = { spec.r, spec.g, spec.b };
    if (ImGui::ColorEdit3("##Specular", specF))
        light->setSpecularColor(kVec3(specF[0], specF[1], specF[2]));

    // Direction — Sun and Spot
    if (lt == LIGHT_TYPE_SUN || lt == LIGHT_TYPE_SPOT)
    {
        propLabel("Direction");
        kVec3 dir = light->getDirection();
        float dirF[3] = { dir.x, dir.y, dir.z };
        if (ImGui::DragFloat3("##Dir", dirF, 0.01f, -1.0f, 1.0f))
            light->setDirection(kVec3(dirF[0], dirF[1], dirF[2]));
    }

    // Attenuation — Point and Spot
    if (lt == LIGHT_TYPE_POINT || lt == LIGHT_TYPE_SPOT)
    {
        propLabel("Constant");
        float c = light->getConstant();
        if (ImGui::DragFloat("##Constant", &c, 0.01f, 0.0f, 10.0f))
            light->setConstant(c);

        propLabel("Linear");
        float l = light->getLinear();
        if (ImGui::DragFloat("##Linear", &l, 0.01f, 0.0f, 10.0f))
            light->setLinear(l);

        propLabel("Quadratic");
        float q = light->getQuadratic();
        if (ImGui::DragFloat("##Quadratic", &q, 0.01f, 0.0f, 10.0f))
            light->setQuadratic(q);
    }

    // Cone angles — Spot only (stored as cosines, displayed as degrees)
    if (lt == LIGHT_TYPE_SPOT)
    {
        propLabel("Inner Cone");
        float inner = glm::degrees(glm::acos(light->getCutOff()));
        if (ImGui::DragFloat("##InnerCone", &inner, 0.5f, 0.0f, 89.0f, "%.1f deg"))
            light->setCutOff(glm::cos(glm::radians(inner)));

        propLabel("Outer Cone");
        float outer = glm::degrees(glm::acos(light->getOuterCutOff()));
        if (ImGui::DragFloat("##OuterCone", &outer, 0.5f, 0.0f, 89.0f, "%.1f deg"))
            light->setOuterCutOff(glm::cos(glm::radians(outer)));
    }

    ImGui::EndTable();
}

// ---------------------------------------------------------------------------
// Main draw
// ---------------------------------------------------------------------------
void PanelInspector::draw(bool &opened)
{
    if (!opened)
        return;

    if (!manager->projectOpened)
        ImGui::BeginDisabled(true);

    gui->windowStart("Inspector", &opened);

    size_t selCount = manager->selectedObjects.size();

    if (selCount == 0)
    {
        // Nothing selected — centred hint
        gui->spacing();
        const char *text = "Nothing is selected";
        float tw = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - tw) * 0.5f);
        ImGui::TextDisabled("%s", text);
    }
    else if (selCount > 1)
    {
        // Multi-selection — show count only
        gui->spacing();
        kString text = std::to_string(selCount) + " objects selected";
        float tw = ImGui::CalcTextSize(text.c_str()).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - tw) * 0.5f);
        ImGui::TextDisabled("%s", text.c_str());
    }
    else
    {
        // Single selection
        kObject *obj = manager->selectedObject;

        if (obj == nullptr)
        {
            ImGui::TextDisabled("(object not found)");
        }
        else
        {
            kNodeType type = obj->getType();

            // --- Header row: type badge + name ---
            const char *typeLabel = "Object";
            if (type == NODE_TYPE_MESH)   typeLabel = "Mesh";
            else if (type == NODE_TYPE_LIGHT)  typeLabel = "Light";
            else if (type == NODE_TYPE_CAMERA) typeLabel = "Camera";

            ImGui::TextDisabled("[%s]", typeLabel);
            ImGui::SameLine();

            // Name — committed on Enter; buffer is refreshed from object each frame
            char nameBuf[256];
            strncpy(nameBuf, obj->getName().c_str(), sizeof(nameBuf) - 1);
            nameBuf[sizeof(nameBuf) - 1] = '\0';
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputText("##ObjName", nameBuf, sizeof(nameBuf),
                                 ImGuiInputTextFlags_EnterReturnsTrue))
                obj->setName(kString(nameBuf));

            // Active checkbox
            bool active = obj->getActive();
            if (ImGui::Checkbox("Active", &active))
                obj->setActive(active);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // --- Transform (all types) ---
            drawTransformSection(obj);

            ImGui::Spacing();

            // --- Type-specific ---
            if (type == NODE_TYPE_MESH)
                drawMeshSection(static_cast<kMesh *>(obj));
            else if (type == NODE_TYPE_LIGHT)
                drawLightSection(static_cast<kLight *>(obj));
            else if (type == NODE_TYPE_CAMERA)
                drawCameraSection(static_cast<kCamera *>(obj));
        }
    }

    gui->windowEnd();

    if (!manager->projectOpened)
        ImGui::EndDisabled();
}
