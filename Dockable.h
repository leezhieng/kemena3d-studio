#ifndef DOCKABLE_H
#define DOCKABLE_H

#include <iostream>

#include <wx/wx.h>
#include <wx/aui/aui.h>

#include "MainWindow.h"

#include "ProjectPane.h"
#include "InspectorPane.h"
#include "HierarchyPane.h"
#include "ScenePane.h"
#include "ConsolePane.h"

#include "ScriptPane.h"

// Called in main.cpp
void SetupDockable(MainWindow* window)
{
    ProjectPane* projectPane = new ProjectPane(window, wxID_ANY);
    {
        projectPane->setMainWindow(window);

        wxBitmap iconBitmap("ICON_FOLDER_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("project"));
        info.Caption(wxT("Project"));
        info.PinButton(true);
        info.DestroyOnClose(false);
        info.Left();
        info.Icon(icon);
        window->AuiManager->AddPane(projectPane, info);
        //notebook->AddPage(projectPane, "Project", true, icon);

        // Setup button icons

        wxBitmap folderUpBitmap("ICON_FOLDER_UP_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage folderUp = folderUpBitmap.ConvertToImage();
        folderUp.Rescale(20, 20, wxIMAGE_QUALITY_BILINEAR);
        projectPane->projectFolderUpButton->SetBitmap(folderUp);

        wxBitmap folderUpPrBitmap("ICON_FOLDER_UP_BUTTON_PRESSED", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage folderUpPr = folderUpPrBitmap.ConvertToImage();
        folderUpPr.Rescale(20, 20, wxIMAGE_QUALITY_BILINEAR);
        projectPane->projectFolderUpButton->SetBitmapPressed(folderUpPr);

        wxBitmap addRoundBitmap("ICON_ADD_ROUND_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage addRound = addRoundBitmap.ConvertToImage();
        addRound.Rescale(17, 17, wxIMAGE_QUALITY_HIGH);
        projectPane->projectAddButton->SetBitmap(addRound);

        wxBitmap addRoundPrBitmap("ICON_ADD_ROUND_BUTTON_PRESSED", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage addRoundPr = addRoundPrBitmap.ConvertToImage();
        addRoundPr.Rescale(17, 17, wxIMAGE_QUALITY_HIGH);
        projectPane->projectAddButton->SetBitmapPressed(addRoundPr);
    }

    InspectorPane* inspectorPane = new InspectorPane(window, wxID_ANY);
    {
        wxBitmap iconBitmap("ICON_INFO_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("inspector"));
        info.Caption(wxT("Inspector"));
        info.PinButton(true);
        info.DestroyOnClose(false);
        info.Right();
        info.Icon(icon);
        window->AuiManager->AddPane(inspectorPane, info);
    }

    HierarchyPane* hierarchyPane = new HierarchyPane(window, wxID_ANY);
    {
        wxBitmap iconBitmap("ICON_HIERARCHY_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("hierarchy"));
        info.Caption(wxT("Hierarchy"));
        info.PinButton(true);
        info.DestroyOnClose(false);
        info.Left();
        info.Icon(icon);
        window->AuiManager->AddPane(hierarchyPane, info);

        // Setup button icons

        wxBitmap addRoundBitmap("ICON_ADD_ROUND_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage addRound = addRoundBitmap.ConvertToImage();
        addRound.Rescale(17, 17, wxIMAGE_QUALITY_HIGH);
        hierarchyPane->hierarchyAddButton->SetBitmap(addRound);

        wxBitmap addRoundPrBitmap("ICON_ADD_ROUND_BUTTON_PRESSED", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage addRoundPr = addRoundPrBitmap.ConvertToImage();
        addRoundPr.Rescale(17, 17, wxIMAGE_QUALITY_HIGH);
        hierarchyPane->hierarchyAddButton->SetBitmapPressed(addRoundPr);
    }

    ScenePane* scenePane = new ScenePane(window, wxID_ANY);
    {
        wxBitmap iconBitmap("ICON_SCENE_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("scene"));
        info.Caption(wxT("Scene"));
        info.MaximizeButton(true);
        info.CloseButton(false);
        info.DestroyOnClose(false);
        info.Center();
        info.Icon(icon);
        window->AuiManager->AddPane(scenePane, info);

        // Setup button icons

        wxImage playButton("ICON_PLAY_BUTTON", wxBITMAP_TYPE_ICO_RESOURCE);
        playButton.Rescale(20, 20, wxIMAGE_QUALITY_BILINEAR);
        scenePane->scenePlayButton->SetBitmap(playButton);

        wxBitmap settingsButtonBitmap("ICON_SETTINGS_BUTTON", wxBITMAP_TYPE_ICO_RESOURCE);
        wxImage settingsButton = settingsButtonBitmap.ConvertToImage();
        settingsButton.Rescale(17, 17, wxIMAGE_QUALITY_BILINEAR);
        scenePane->sceneSettingsButton->SetBitmap(settingsButton);
    }

    ConsolePane* consolePane = new ConsolePane(window, wxID_ANY);
    {
        wxBitmap iconBitmap("ICON_TERMINAL_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("console"));
        info.Caption(wxT("Console"));
        info.PinButton(true);
        info.DestroyOnClose(false);
        info.Bottom();
        info.Icon(icon);
        window->AuiManager->AddPane(consolePane, info);
    }

    ScriptPane* scriptPane = new ScriptPane(window, wxID_ANY);
    {
        wxBitmap iconBitmap("ICON_SCRIPT_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage img = iconBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        wxIcon icon;
        icon.CopyFromBitmap(rescaled);

        wxAuiPaneInfo info;
        info.Name(wxT("script"));
        info.Caption(wxT("Script Editor"));
        info.PinButton(true);
        info.DestroyOnClose(false);
        info.Right();
        info.Icon(icon);
        window->AuiManager->AddPane(scriptPane, info);
    }

    window->AuiManager->Update();
}

// Called in mainmenu.cpp
void TogglePane(MainWindow* window, wxString name)
{
    wxAuiPaneInfo &info = window->AuiManager->GetPane(name);
    if (info.IsShown())
        info.Hide();
    else
        info.Show();
    window->AuiManager->Update();
}

// Called in mainmenu.cpp
void ResetWorkspace(MainWindow* window)
{
    wxAuiPaneInfoArray panes = window->AuiManager->GetAllPanes();

    for (size_t i = 0; i < panes.size(); ++i)
    {
        wxString name = panes[i].name;
		wxAuiPaneInfo &info = window->AuiManager->GetPane(name);

		if (name == wxT("project"))
        {
            info.Left();
            info.Dock();
            info.Show();
        }
        else if (name == wxT("inspector"))
        {
            info.Right();
            info.Dock();
            info.Show();
        }
        else if (name == wxT("hierarchy"))
        {
            info.Left();
            info.Dock();
            info.Show();
        }
        else if (name == wxT("scene"))
        {
            info.Center();
            info.Dock();
            info.Show();
        }
        else if (name == wxT("console"))
        {
            info.Bottom();
            info.Dock();
            info.Show();
        }
        else
        {
            info.Dock();
            info.Hide();
        }
    }

    window->AuiManager->Update();
}

#endif // header guard

