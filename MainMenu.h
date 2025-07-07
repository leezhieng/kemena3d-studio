#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <wx/wx.h>
#include "MainWindow.h"

#include "Dockable.h"

enum MenuID
{
    // File
    MENU_FILE_NEW_SCENE,
    MENU_FILE_OPEN_SCENE,
    MENU_FILE_OPEN_RECENT_SCENE,
    MENU_FILE_SAVE,
    MENU_FILE_SAVE_AS,
    MENU_FILE_NEW_PROJECT,
    MENU_FILE_OPEN_PROJECT,
    MENU_FILE_SAVE_PROJECT,
    MENU_FILE_BUILD_SETTINGS,
    MENU_FILE_BUILD_AND_RUN,
    MENU_FILE_EXIT,
    // Edit
    MENU_EDIT_UNDO,
    MENU_EDIT_REDO,
    MENU_EDIT_UNDO_HISTORY,
    MENU_EDIT_SELECT_ALL,
    MENU_EDIT_DESELECT_ALL,
    MENU_EDIT_SELECT_CHILDREN,
    MENU_EDIT_SELECT_PREFAB_ROOT,
    MENU_EDIT_INVERT_SELECTION,
    MENU_EDIT_CUT,
    MENU_EDIT_COPY,
    MENU_EDIT_PASTE,
    MENU_EDIT_PASTE_AS_CHILD,
    MENU_EDIT_DUPLICATE,
    MENU_EDIT_RENAME,
    MENU_EDIT_DELETE,
    MENU_EDIT_FRAME_SELECTED,
    MENU_EDIT_LOCK_VIEW_TO_SELECTED,
    MENU_EDIT_PLAY,
    MENU_EDIT_PAUSE,
    MENU_EDIT_SIGN_IN,
    MENU_EDIT_SIGN_OUT,
    MENU_EDIT_PROJECT_SETTINGS,
    MENU_EDIT_PREFERENCES,
    MENU_EDIT_SHORTCUTS,
    MENU_EDIT_CLEAR_ALL_PLAYERPREFS,
    // Assets
    MENU_ASSETS_CREATE,
    MENU_ASSETS_SHOW_IN_EXPLORER,
    MENU_ASSETS_OPEN,
    MENU_ASSETS_DELETE,
    MENU_ASSETS_RENAME,
    MENU_ASSETS_COPY_PATH,
    MENU_ASSETS_REFRESH,
    MENU_ASSETS_REIMPORT,
    MENU_ASSETS_REIMPORT_ALL,
    MENU_ASSETS_GENERATE_LIGHTING,
    // Object
    MENU_OBJECT_CREATE_EMPTY_PARENT,
    MENU_OBJECT_CREATE_EMPTY_CHILD,
    MENU_OBJECT_CREATE_EMPTY,
    MENU_OBJECT_3D_OBJECT,
    MENU_OBJECT_EFFECTS,
    MENU_OBJECT_LIGHT,
    MENU_OBJECT_AUDIO,
    MENU_OBJECT_VIDEO,
    MENU_OBJECT_UI,
    MENU_OBJECT_CAMERA,
    // Component
    MENU_COMPONENT_AUDIO,
    MENU_COMPONENT_EFFECT,
    MENU_COMPONENT_MESH,
    MENU_COMPONENT_PHYSICS,
    MENU_COMPONENT_SCRIPTS,
    // Window
    MENU_WINDOW_GENERAL,
        MENU_WINDOW_GENERAL_INSPECTOR,
        MENU_WINDOW_GENERAL_HIERARCHY,
        MENU_WINDOW_GENERAL_PROJECT,
        MENU_WINDOW_GENERAL_SCRIPT,
        MENU_WINDOW_GENERAL_CONSOLE,
    MENU_WINDOW_RENDERING,
    MENU_WINDOW_ANIMATION,
    MENU_WINDOW_AUDIO,
    MENU_WINDOW_SEQUENCING,
    MENU_WINDOW_WORKSPACE,
        MENU_WINDOW_WORKSPACE_SAVE,
        MENU_WINDOW_WORKSPACE_LOAD,
        MENU_WINDOW_WORKSPACE_RESET,
    // Help
    MENU_HELP_ABOUT,
    MENU_HELP_MANUAL,
    MENU_HELP_SCRIPTING_REFERENCE,
    MENU_HELP_RELEASE_NOTES,
    MENU_HELP_SOFTWARE_LICENSES,
    MENU_HELP_REPORT_A_BUG,
};

// Called in main.cpp
void SetupMainMenu(MainWindow* window)
{
    wxMenuBar* menuBar = new wxMenuBar();

    // File
    wxMenu* fileMenu = new wxMenu();
    {
        fileMenu->Append(MENU_FILE_NEW_SCENE, wxT("New Scene"));
        fileMenu->Append(MENU_FILE_OPEN_SCENE, wxT("Open Scene"));
        fileMenu->Append(MENU_FILE_OPEN_RECENT_SCENE, wxT("Open Recent Scene"));
        fileMenu->AppendSeparator();
        fileMenu->Append(MENU_FILE_SAVE, wxT("Save"));
        fileMenu->Append(MENU_FILE_SAVE_AS, wxT("Save As..."));
        fileMenu->AppendSeparator();
        fileMenu->Append(MENU_FILE_NEW_PROJECT, wxT("New Project"));
        {
            fileMenu->Bind(wxEVT_MENU, &MainWindow::OnNewProject, window, MENU_FILE_NEW_PROJECT);
        }
        fileMenu->Append(MENU_FILE_OPEN_PROJECT, wxT("Open Project"));
        {
            fileMenu->Bind(wxEVT_MENU, &MainWindow::OnOpenProject, window, MENU_FILE_OPEN_PROJECT);
        }
        fileMenu->Append(MENU_FILE_SAVE_PROJECT, wxT("Save Project"));
        fileMenu->AppendSeparator();
        fileMenu->Append(MENU_FILE_BUILD_SETTINGS, wxT("Build Settings"));
        fileMenu->Append(MENU_FILE_BUILD_AND_RUN, wxT("Build And Run"));
        fileMenu->AppendSeparator();
        fileMenu->Append(MENU_FILE_EXIT, wxT("Exit"));
        {
            fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { window->Close(true); }, MENU_FILE_EXIT, wxID_ANY);
        }

        // Disabled by default
        fileMenu->Enable(MENU_FILE_NEW_SCENE, false);
        fileMenu->Enable(MENU_FILE_OPEN_SCENE, false);
        fileMenu->Enable(MENU_FILE_OPEN_RECENT_SCENE, false);
        fileMenu->Enable(MENU_FILE_SAVE, false);
        fileMenu->Enable(MENU_FILE_SAVE_AS, false);
        fileMenu->Enable(MENU_FILE_SAVE_PROJECT, false);
        fileMenu->Enable(MENU_FILE_BUILD_SETTINGS, false);
        fileMenu->Enable(MENU_FILE_BUILD_AND_RUN, false);
    }
    menuBar->Append(fileMenu, wxT("File"));

    // Edit
    wxMenu* editMenu = new wxMenu();
    {
        editMenu->Append(MENU_EDIT_UNDO, wxT("Undo"));
        editMenu->Append(MENU_EDIT_REDO, wxT("Redo"));
        editMenu->Append(MENU_EDIT_UNDO_HISTORY, wxT("Undo History"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_SELECT_ALL, wxT("Select All"));
        editMenu->Append(MENU_EDIT_DESELECT_ALL, wxT("Deselect All"));
        editMenu->Append(MENU_EDIT_SELECT_CHILDREN, wxT("Select Children"));
        editMenu->Append(MENU_EDIT_SELECT_PREFAB_ROOT, wxT("Select Prefab Root"));
        editMenu->Append(MENU_EDIT_INVERT_SELECTION, wxT("Invert Selection"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_CUT, wxT("Cut"));
        editMenu->Append(MENU_EDIT_COPY, wxT("Copy"));
        editMenu->Append(MENU_EDIT_PASTE, wxT("Paste"));
        editMenu->Append(MENU_EDIT_PASTE_AS_CHILD, wxT("Paste As Child"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_DUPLICATE, wxT("Duplicate"));
        editMenu->Append(MENU_EDIT_RENAME, wxT("Rename"));
        editMenu->Append(MENU_EDIT_DELETE, wxT("Delete"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_FRAME_SELECTED, wxT("Frame Selected"));
        editMenu->Append(MENU_EDIT_LOCK_VIEW_TO_SELECTED, wxT("Lock View To Selected"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_PLAY, wxT("Play"));
        editMenu->Append(MENU_EDIT_PAUSE, wxT("Pause"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_SIGN_IN, wxT("Sign In"));
        editMenu->Append(MENU_EDIT_SIGN_OUT, wxT("Sign Out"));
        editMenu->AppendSeparator();
        editMenu->Append(MENU_EDIT_PROJECT_SETTINGS, wxT("Project Settings"));
        editMenu->Append(MENU_EDIT_PREFERENCES, wxT("Preferences"));
        editMenu->Append(MENU_EDIT_SHORTCUTS, wxT("Shortcuts"));
        editMenu->Append(MENU_EDIT_CLEAR_ALL_PLAYERPREFS, wxT("Clear All PlayerPrefs"));

        // Disabled by default
        editMenu->Enable(MENU_EDIT_UNDO, false);
        editMenu->Enable(MENU_EDIT_REDO, false);
        editMenu->Enable(MENU_EDIT_UNDO_HISTORY, false);
        editMenu->Enable(MENU_EDIT_SELECT_ALL, false);
        editMenu->Enable(MENU_EDIT_DESELECT_ALL, false);
        editMenu->Enable(MENU_EDIT_SELECT_CHILDREN, false);
        editMenu->Enable(MENU_EDIT_SELECT_PREFAB_ROOT, false);
        editMenu->Enable(MENU_EDIT_INVERT_SELECTION, false);
        editMenu->Enable(MENU_EDIT_DESELECT_ALL, false);
        editMenu->Enable(MENU_EDIT_CUT, false);
        editMenu->Enable(MENU_EDIT_COPY, false);
        editMenu->Enable(MENU_EDIT_PASTE, false);
        editMenu->Enable(MENU_EDIT_PASTE_AS_CHILD, false);
        editMenu->Enable(MENU_EDIT_DUPLICATE, false);
        editMenu->Enable(MENU_EDIT_RENAME, false);
        editMenu->Enable(MENU_EDIT_DELETE, false);
        editMenu->Enable(MENU_EDIT_FRAME_SELECTED, false);
        editMenu->Enable(MENU_EDIT_LOCK_VIEW_TO_SELECTED, false);
        editMenu->Enable(MENU_EDIT_PLAY, false);
        editMenu->Enable(MENU_EDIT_PAUSE, false);
        editMenu->Enable(MENU_EDIT_PROJECT_SETTINGS, false);
        editMenu->Enable(MENU_EDIT_CLEAR_ALL_PLAYERPREFS, false);
    }
    menuBar->Append(editMenu, wxT("Edit"));

    // Assets
    wxMenu* assetsMenu = new wxMenu();
    {
        assetsMenu->Append(MENU_ASSETS_CREATE, wxT("Create"));
        assetsMenu->Append(MENU_ASSETS_SHOW_IN_EXPLORER, wxT("Show In Explorer"));
        assetsMenu->Append(MENU_ASSETS_OPEN, wxT("Open"));
        assetsMenu->Append(MENU_ASSETS_DELETE, wxT("Delete"));
        assetsMenu->Append(MENU_ASSETS_RENAME, wxT("Rename"));
        assetsMenu->Append(MENU_ASSETS_COPY_PATH, wxT("Copy Path"));
        assetsMenu->AppendSeparator();
        assetsMenu->Append(MENU_ASSETS_REFRESH, wxT("Refresh"));
        assetsMenu->Append(MENU_ASSETS_REIMPORT, wxT("Reimport"));
        assetsMenu->AppendSeparator();
        assetsMenu->Append(MENU_ASSETS_REIMPORT_ALL, wxT("Reimport All"));
        assetsMenu->AppendSeparator();
        assetsMenu->Append(MENU_ASSETS_GENERATE_LIGHTING, wxT("Generate Lighting"));

        assetsMenu->Enable(MENU_ASSETS_CREATE, false);
        assetsMenu->Enable(MENU_ASSETS_SHOW_IN_EXPLORER, false);
        assetsMenu->Enable(MENU_ASSETS_OPEN, false);
        assetsMenu->Enable(MENU_ASSETS_DELETE, false);
        assetsMenu->Enable(MENU_ASSETS_RENAME, false);
        assetsMenu->Enable(MENU_ASSETS_COPY_PATH, false);
        assetsMenu->Enable(MENU_ASSETS_REFRESH, false);
        assetsMenu->Enable(MENU_ASSETS_REIMPORT, false);
        assetsMenu->Enable(MENU_ASSETS_REIMPORT_ALL, false);
        assetsMenu->Enable(MENU_ASSETS_GENERATE_LIGHTING, false);
    }
    menuBar->Append(assetsMenu, wxT("Assets"));

    // Object
    wxMenu* objectMenu = new wxMenu();
    {
        objectMenu->Append(MENU_OBJECT_CREATE_EMPTY_PARENT, wxT("Create Empty Parent"));
        objectMenu->Append(MENU_OBJECT_CREATE_EMPTY_CHILD, wxT("Create Empty Child"));
        objectMenu->Append(MENU_OBJECT_CREATE_EMPTY, wxT("Create Empty"));
        objectMenu->Append(MENU_OBJECT_3D_OBJECT, wxT("3D Object"));
        objectMenu->Append(MENU_OBJECT_EFFECTS, wxT("Effects"));
        objectMenu->Append(MENU_OBJECT_LIGHT, wxT("Light"));
        objectMenu->Append(MENU_OBJECT_AUDIO, wxT("Audio"));
        objectMenu->Append(MENU_OBJECT_VIDEO, wxT("Video"));
        objectMenu->Append(MENU_OBJECT_UI, wxT("UI"));
        objectMenu->Append(MENU_OBJECT_CAMERA, wxT("Camera"));

        objectMenu->Enable(MENU_OBJECT_CREATE_EMPTY_PARENT, false);
        objectMenu->Enable(MENU_OBJECT_CREATE_EMPTY_CHILD, false);
        objectMenu->Enable(MENU_OBJECT_CREATE_EMPTY, false);
        objectMenu->Enable(MENU_OBJECT_3D_OBJECT, false);
        objectMenu->Enable(MENU_OBJECT_EFFECTS, false);
        objectMenu->Enable(MENU_OBJECT_LIGHT, false);
        objectMenu->Enable(MENU_OBJECT_AUDIO, false);
        objectMenu->Enable(MENU_OBJECT_VIDEO, false);
        objectMenu->Enable(MENU_OBJECT_UI, false);
        objectMenu->Enable(MENU_OBJECT_CAMERA, false);
    }
    menuBar->Append(objectMenu, wxT("Object"));
    window->objectMenu = objectMenu;

    // Component
    wxMenu* componentMenu = new wxMenu();
    {
        componentMenu->Append(MENU_COMPONENT_AUDIO, wxT("Audio"));
        componentMenu->Append(MENU_COMPONENT_EFFECT, wxT("Effect"));
        componentMenu->Append(MENU_COMPONENT_MESH, wxT("Mesh"));
        componentMenu->Append(MENU_COMPONENT_PHYSICS, wxT("Physics"));
        componentMenu->Append(MENU_COMPONENT_SCRIPTS, wxT("Scripts"));

        componentMenu->Enable(MENU_COMPONENT_AUDIO, false);
        componentMenu->Enable(MENU_COMPONENT_EFFECT, false);
        componentMenu->Enable(MENU_COMPONENT_MESH, false);
        componentMenu->Enable(MENU_COMPONENT_PHYSICS, false);
        componentMenu->Enable(MENU_COMPONENT_SCRIPTS, false);
    }
    menuBar->Append(componentMenu, wxT("Component"));

    // Window
    wxMenu* windowMenu = new wxMenu();
    {
        wxMenu* generalMenu = new wxMenu();
        windowMenu->Append(MENU_WINDOW_GENERAL, wxT("General"), generalMenu);
        {
            generalMenu->Append(MENU_WINDOW_GENERAL_INSPECTOR, wxT("Inspector"));
            {
                generalMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { TogglePane(window, wxT("inspector")); }, MENU_WINDOW_GENERAL_INSPECTOR, wxID_ANY);
            }
            generalMenu->Append(MENU_WINDOW_GENERAL_HIERARCHY, wxT("Hierarchy"));
            {
                generalMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { TogglePane(window, wxT("hierarchy")); }, MENU_WINDOW_GENERAL_HIERARCHY, wxID_ANY);
            }
            generalMenu->Append(MENU_WINDOW_GENERAL_PROJECT, wxT("Project"));
            {
                generalMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { TogglePane(window, wxT("project")); }, MENU_WINDOW_GENERAL_PROJECT, wxID_ANY);
            }
            generalMenu->Append(MENU_WINDOW_GENERAL_SCRIPT, wxT("Script Editor"));
            {
                generalMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { TogglePane(window, wxT("script")); }, MENU_WINDOW_GENERAL_SCRIPT, wxID_ANY);
            }
            generalMenu->Append(MENU_WINDOW_GENERAL_CONSOLE, wxT("Console"));
            {
                generalMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { TogglePane(window, wxT("console")); }, MENU_WINDOW_GENERAL_CONSOLE, wxID_ANY);
            }
        }
        windowMenu->Append(MENU_WINDOW_RENDERING, wxT("Rendering"));
        windowMenu->Append(MENU_WINDOW_ANIMATION, wxT("Animation"));
        windowMenu->Append(MENU_WINDOW_AUDIO, wxT("Audio"));
        windowMenu->Append(MENU_WINDOW_SEQUENCING, wxT("Sequencing"));
        windowMenu->AppendSeparator();
        wxMenu* workspaceMenu = new wxMenu();
        windowMenu->Append(MENU_WINDOW_WORKSPACE, wxT("Workspace"), workspaceMenu);
        {
            workspaceMenu->Append(MENU_WINDOW_WORKSPACE_SAVE, wxT("Save"));
            workspaceMenu->Append(MENU_WINDOW_WORKSPACE_LOAD, wxT("Load"));
            workspaceMenu->AppendSeparator();
            workspaceMenu->Append(MENU_WINDOW_WORKSPACE_RESET, wxT("Reset"));
            {
                workspaceMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, [=](wxCommandEvent&) { ResetWorkspace(window); }, MENU_WINDOW_WORKSPACE_RESET, wxID_ANY);
            }
        }
    }
    menuBar->Append(windowMenu, wxT("Window"));

    // Help
    wxMenu* helpMenu = new wxMenu();
    {
        helpMenu->Append(MENU_HELP_ABOUT, wxT("About"));
        helpMenu->AppendSeparator();
        helpMenu->Append(MENU_HELP_MANUAL, wxT("Manual"));
        helpMenu->Append(MENU_HELP_SCRIPTING_REFERENCE, wxT("Scripting Reference"));
        helpMenu->AppendSeparator();
        helpMenu->Append(MENU_HELP_RELEASE_NOTES, wxT("Release Notes"));
        helpMenu->Append(MENU_HELP_SOFTWARE_LICENSES, wxT("Software Licenses"));
        helpMenu->Append(MENU_HELP_REPORT_A_BUG, wxT("Report a Bug"));
    }
    menuBar->Append(helpMenu, wxT("Help"));

    window->SetMenuBar(menuBar);
}

#endif // header guard

