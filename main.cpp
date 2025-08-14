#include "kemena/kemena.h"

using namespace kemena;

int main()
{
    // Create window and renderer
    kWindow* window = createWindow(1024, 768, "Kemena3D Studio");
    kRenderer* renderer = createRenderer(window);
    renderer->setClearColor(vec4(0.0f, 0.0f, 0.0f, 1.0f));

    // Create the asset manager, world and scene
    kAssetManager* assetManager = createAssetManager();
    kWorld* world = createWorld(assetManager);
    kScene* scene = world->createScene("My Scene");

    kCamera* camera = scene->addCamera(glm::vec3(-20.0f, 5.0f, 20.0f), glm::vec3(0.0f, 0.5f, 0.0f), kCameraType::CAMERA_TYPE_LOCKED);

    kGuiManager* gui = createGuiManager(renderer);

    // Game loop
    kSystemEvent event;
    while (window->getRunning())
    {
        gui->processEvent(event);

        if (event.hasEvent())
        {
            if (event.getType() == K_EVENT_QUIT)
            {
                window->setRunning(false);
            }
        }

        renderer->render(scene, 0, 0, window->getWindowWidth(), window->getWindowHeight(), window->getTimer()->getDeltaTime(), false);

        gui->canvasStart();
        gui->dockSpaceStart("MainDockSpace");




        // Draw main menu bar
        if (gui->menuBar())
        {
            // File menu
            if (gui->menu("File"))
            {
                if (gui->menuItem("New", "Ctrl+N")) { /* Handle new */ }
                if (gui->menuItem("Open...", "Ctrl+O")) { /* Handle open */ }
                if (gui->menuItem("Save", "Ctrl+S")) { /* Handle save */ }
                gui->separator();
                if (gui->menuItem("Exit")) { /* Handle exit */ }
                gui->menuEnd();
            }

            // Edit menu
            if (gui->menu("Edit"))
            {
                if (gui->menuItem("Undo", "Ctrl+Z")) {}
                if (gui->menuItem("Redo", "Ctrl+Y", false, false)) {} // disabled item
                gui->separator();
                if (gui->menuItem("Cut", "Ctrl+X")) {}
                if (gui->menuItem("Copy", "Ctrl+C")) {}
                if (gui->menuItem("Paste", "Ctrl+V")) {}
                gui->menuEnd();
            }

            // View menu
            if (gui->menu("View"))
            {
                static bool showInspector = true;
                if (gui->menuItem("Inspector", "", showInspector))
                    showInspector = !showInspector;

                static bool showConsole = true;
                if (gui->menuItem("Console", "", showConsole))
                    showConsole = !showConsole;

                gui->menuEnd();
            }

            gui->menuBarEnd();
        }



        gui->windowStart("Inspector");

        // --- Icon & Right-side group ---
        {
            // Icon on the left
            gui->groupStart();
            ImGui::Button("Icon", ImVec2(48, 48)); // Replace with ImGui::Image for real icon
            gui->groupEnd();

            gui->sameLine();

            // Right side (name + checkboxes)
            gui->groupStart();

            // Name field
            static char name[128] = "My Object";
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("##Name", name, IM_ARRAYSIZE(name));

            // Visible + Static in the same row
            static bool enabled = true;
            static bool isStatic = false;
            ImGui::Checkbox("Enabled", &enabled);
            gui->sameLine();
            ImGui::Checkbox("Static", &isStatic);

            gui->groupEnd();
        }

        gui->spacing();

        // --- Transform section ---
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static float position[3] = {0.0f, 0.0f, 0.0f};
            static float rotation[3] = {0.0f, 0.0f, 0.0f};
            static float scale[3]    = {1.0f, 1.0f, 1.0f};

            if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

                // Position
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Position");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat3("##Position", position, 0.1f);

                // Rotation
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Rotation");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat3("##Rotation", rotation, 0.1f);

                // Scale
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Scale");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::DragFloat3("##Scale", scale, 0.1f);

                ImGui::EndTable();
            }
        }

        gui->windowEnd();


        gui->dockSpaceEnd();
        gui->canvasEnd();

        window->swap();
    }

    // Clean up
    gui->destroy();
    renderer->destroy();
    window->destroy();
    return 0;
}
