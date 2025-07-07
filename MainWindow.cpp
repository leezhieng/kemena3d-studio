#include "MainWindow.h"

//(*InternalHeaders(MainWindow)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(MainWindow)
//*)

BEGIN_EVENT_TABLE(MainWindow,wxFrame)
    //(*EventTable(MainWindow)
    //*)
END_EVENT_TABLE()

MainWindow::MainWindow(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(MainWindow)
    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(800,600));
    Move(wxDefaultPosition);
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    AuiManager = new wxAuiManager(this, wxAUI_MGR_LIVE_RESIZE|wxAUI_MGR_DEFAULT);
    Center();
    //*)

    updateWindowTitle();
    fileManager = new FileManager();
}

MainWindow::~MainWindow()
{
    //(*Destroy(MainWindow)
    //*)
}

void MainWindow::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive())
    {
        /*
        // Check for file changes whenever refocusing back to the editor
        std::thread([this]()
        {
            //fileManager->checkAssetsChange(fileManager->projectPath + wxFILE_SEP_PATH + "Assets");
        }).detach(); // Detach the thread so it runs independently

        // Refresh project list
        std::thread([this]()
        {
            //fileManager->checkAssetsChange(fileManager->projectPath + wxFILE_SEP_PATH + "Assets");
        }).detach(); // Detach the thread so it runs independently
        */
    }
    else
    {
    }

    event.Skip(); // Let the event continue to other handlers
}

void MainWindow::OnNewProject(wxCommandEvent& event)
{
    NewProjectDialog dialog(this);
    if (dialog.ShowModal() == wxID_SAVE)
    {
        bool success = fileManager->newProject(dialog.nameInput->GetValue(), dialog.locationInput->GetPath());

        if (success)
        {
            projectName = dialog.nameInput->GetValue().Strip();
            projectSaved = false;
            updateWindowTitle();

            newWorld();

            setEnableAllPanes(true);
            setEnableAllMenus(true);

            refreshProjectList();
            loadSceneList();
        }
    }
}

void MainWindow::OnOpenProject(wxCommandEvent& event)
{
    wxDirDialog dialog(
        nullptr,
        "Open Project",         // Dialog title
        "",                     // Default path
        wxDD_DIR_MUST_EXIST | wxDD_DEFAULT_STYLE
    );

    if (dialog.ShowModal() == wxID_OK)
    {
        wxString selectedPath = dialog.GetPath();
        wxFileName fileName(selectedPath);
        wxString lastFolder = fileName.GetDirs().Last();

        bool success = fileManager->openProject(selectedPath);

        if (success)
        {
            projectName = lastFolder.Strip();
            projectSaved = false;
            updateWindowTitle();

            // Load previously opened world if exist
            //loadWorld("");

            // Create new world if old world doesn't exist
            newWorld();

            setEnableAllPanes(true);
            setEnableAllMenus(true);

            refreshProjectList();
            // WIP: Load scenes of the world
            loadSceneList();
        }
    }
}

void MainWindow::OnHierarchyAddButtonClicked(wxCommandEvent& event)
{
    wxMenu* clonedMenu = cloneMenu(objectMenu);
    PopupMenu(clonedMenu, wxGetMousePosition());

    delete clonedMenu;
}

wxMenu* MainWindow::cloneMenu(wxMenu* sourceMenu)
{
    wxMenu* newMenu = new wxMenu;

    wxMenuItemList items = sourceMenu->GetMenuItems();
    for (auto item : items)
    {
        if (item->IsSeparator())
        {
            newMenu->AppendSeparator();
        }
        else if (item->GetSubMenu())
        {
            // Recursively clone the submenu
            wxMenu* clonedSubmenu = cloneMenu(item->GetSubMenu());
            newMenu->AppendSubMenu(clonedSubmenu, item->GetItemLabel(), item->GetHelp());
        }
        else
        {
            newMenu->Append(item->GetId(), item->GetItemLabel(), item->GetHelp());
        }
    }

    return newMenu;
}

void MainWindow::setEnableAllPanes(bool enabled)
{
    wxAuiPaneInfoArray& allPanes = AuiManager->GetAllPanes();

    for (size_t i = 0; i < allPanes.GetCount(); ++i)
    {
        wxAuiPaneInfo& pane = allPanes.Item(i);
        pane.window->Enable(enabled); // Enable the pane
        //pane.Show(enabled);    // Make sure it's visible
    }

    AuiManager->Update(); // Apply the changes;
}

void MainWindow::setEnableAllMenus(bool enabled)
{
    wxMenuBar* menuBar = GetMenuBar(); // or your specific pointer to the menu bar

    if (menuBar)
    {
        int count = menuBar->GetMenuCount();
        for (int i = 0; i < count; ++i)
        {
            wxMenu* menu = menuBar->GetMenu(i);
            if (menu)
            {
                size_t itemCount = menu->GetMenuItemCount();
                for (size_t j = 0; j < itemCount; ++j)
                {
                    wxMenuItem* item = menu->FindItemByPosition(j);
                    if (item && item->IsEnabled() != enabled)
                    {
                        menu->Enable(item->GetId(), enabled);
                    }
                }
            }
        }
    }
}

void MainWindow::refreshProjectList()
{
    ProjectPane* projectPane = nullptr;
    wxAuiPaneInfo& projectPaneInfo = AuiManager->GetPane("project");
    if (projectPaneInfo.IsOk())
    {
        projectPane = dynamic_cast<ProjectPane*>(projectPaneInfo.window);
    }

    if (projectPane != nullptr)
    {
        std::string path = fileManager->projectPath;
        if (fileManager->currentDir.size() > 0)
        {
            for (size_t i = 0; i < fileManager->currentDir.size(); ++i)
            {
                path.append(wxFILE_SEP_PATH + fileManager->currentDir.at(i));
            }
        }
        projectPane->displayDirectory(path);

        //std::cout << path << std::endl;
    }
}

void MainWindow::loadSceneList()
{
    HierarchyPane* hierarchyPane = nullptr;
    wxAuiPaneInfo& hierarchyPaneInfo = AuiManager->GetPane("hierarchy");
    if (hierarchyPaneInfo.IsOk())
    {
        hierarchyPane = dynamic_cast<HierarchyPane*>(hierarchyPaneInfo.window);
    }

    ScenePane* scenePane = nullptr;
    wxAuiPaneInfo& scenePaneInfo = AuiManager->GetPane("scene");
    if (scenePaneInfo.IsOk())
    {
        scenePane = dynamic_cast<ScenePane*>(scenePaneInfo.window);
    }

    // Clear list
    hierarchyPane->sceneList->DeleteAllItems();

    // Add root item
    wxTreeItemId rootItem = hierarchyPane->sceneList->AddRoot("World",hierarchyPane->iconIndexWorld);

    // Loop through scenes
    // Start from 1 because we don't want to show what's in the editor scene
    if (scenePane->getRendererWindow()->getWorld()->getScenes().size() > 1)
    {
        for (size_t i = 1; i < scenePane->getRendererWindow()->getWorld()->getScenes().size(); ++i)
        {
            kScene* scene = scenePane->getRendererWindow()->getWorld()->getScenes().at(i);

            // Add scene to list
            wxTreeItemId sceneItem = hierarchyPane->sceneList->AppendItem(rootItem, scene->getName(), hierarchyPane->iconIndexScene);
            //std::cout << scene->getName() << std::endl;

            // Loop through objects in the scene
            kObject* rootNode = scene->getRootNode();
            if (rootNode->getChildren().size() > 0)
            {
                for (size_t j = 0; j < rootNode->getChildren().size(); ++j)
                {
                    kObject* childNode = rootNode->getChildren().at(j);
                    int icon;

                    // WIP: Save the list item to the kObject?

                    if (childNode->getType() == NodeType::NODE_TYPE_OBJECT)
                    {
                        // Empty node
                        icon = hierarchyPane->iconIndexObject;
                    }
                    else if (childNode->getType() == NodeType::NODE_TYPE_MESH)
                    {
                        // Mesh
                        icon = hierarchyPane->iconIndexMesh;
                    }
                    else if (childNode->getType() == NodeType::NODE_TYPE_LIGHT)
                    {
                        // Light
                        icon = hierarchyPane->iconIndexLight;
                    }
                    else if (childNode->getType() == NodeType::NODE_TYPE_CAMERA)
                    {
                        // Camera
                        icon = hierarchyPane->iconIndexCamera;
                    }

                    wxTreeItemId objectItem = hierarchyPane->sceneList->AppendItem(sceneItem, childNode->getName(), icon);
                }
            }
        }
    }

    hierarchyPane->sceneList->Expand(rootItem);
}

void MainWindow::openFolder(wxString name)
{
    fileManager->currentDir.push_back(name.ToStdString());
    refreshProjectList();
}

void MainWindow::closeFolder()
{
    if (fileManager->currentDir.size() > 1)
    {
        fileManager->currentDir.pop_back();
        refreshProjectList();
    }
}

void MainWindow::updateWindowTitle()
{
    if (projectName != "")
    {
        if (worldName == "")
        {
            // Unsaved new world
            if (projectSaved)
                SetTitle(windowTitle + " - " + projectName + " - New World");
            else
                SetTitle(windowTitle + " - " + projectName + " - New World*");
        }
        else
        {
            // Existing world
            if (projectSaved)
                SetTitle(windowTitle + " - " + projectName + " - " + worldName);
            else
                SetTitle(windowTitle + " - " + projectName + " - " + worldName + "*");
        }
    }
    else
    {
        SetTitle(windowTitle);
    }
}

void MainWindow::newWorld()
{
    ScenePane* scenePane = nullptr;
    wxAuiPaneInfo& scenePaneInfo = AuiManager->GetPane("scene");
    if (scenePaneInfo.IsOk())
    {
        scenePane = dynamic_cast<ScenePane*>(scenePaneInfo.window);
    }

    RendererWindow* renderer = scenePane->getRendererWindow();

    // Close old world if exist
    worldName = "";
    //renderer->getSceneManager()->clearScenes();

    // New scene
    renderer->newScene();
}

void MainWindow::loadWorld(std::string fileName)
{
    ScenePane* scenePane = nullptr;
    wxAuiPaneInfo& scenePaneInfo = AuiManager->GetPane("scene");
    if (scenePaneInfo.IsOk())
    {
        scenePane = dynamic_cast<ScenePane*>(scenePaneInfo.window);
    }

    RendererWindow* renderer = scenePane->getRendererWindow();

    // Close old world if exist
    worldName = "";
    //renderer->getSceneManager()->clearScenes();

    // Load world
}

FileManager* MainWindow::getFileManager()
{
    return fileManager;
}
