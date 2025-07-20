#include <iostream>

#include <wx/wx.h>
#include <wx/artprov.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include "dep/wxMaterialDesignArtProvider/MaterialDesign/wxMaterialDesignArtProvider.hpp"

#include "ArtProvider.h"
#include "MainWindow.h"
#include "MainMenu.h"
#include "Dockable.h"

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    wxInitAllImageHandlers();   // Needed for loading PNG images
    wxArtProvider::Push(new wxMaterialDesignArtProvider);

    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxFileName exeFile(exePath);
    wxString baseDir = exeFile.GetPath();

    // Set custom fonts
    wxFont fontInter;
    fontInter.AddPrivateFont(baseDir + "\\fonts\\inter.ttf");
    fontInter.SetFaceName("Inter");

    // Setup main window
    MainWindow* window = new MainWindow(nullptr, wxID_ANY);
    SetTopWindow(window);

    window->AuiManager->SetManagedWindow(window);
    window->SetIcon(wxIcon(wxT("APP_ICON")));
    window->SetFont(fontInter);

    // Set style
    wxAuiManager* auiManager = window->AuiManager;

    ArtProvider* art = new ArtProvider();
    auiManager->SetArtProvider(art);

    // Panel title bar color
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(240, 240, 240));                 // Top color
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(220, 220, 220));        // Bottom color
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(0, 0, 0));                  // Text color
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(240, 240, 240));                   // Top color
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(220, 220, 220));          // Bottom color
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxColour(0, 0, 0));                    // Text color
    art->SetColour(wxAUI_DOCKART_BORDER_COLOUR, wxColour(240, 240, 240));                           // Border color
    art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);                                                     // Border margin for resizing panel
    art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 25);                                                 // Title bar vertical height
    art->SetMetric(wxAUI_DOCKART_PANE_BUTTON_SIZE, 25);                                             // Button size, horizontally
    art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);                                              // Panel's padding
    art->SetFont(wxAUI_DOCKART_CAPTION_FONT, fontInter);                                            // Caption font

    // Apply the changes
    auiManager->Update();

    // Setup dockable panes
    SetupDockable(window);

    // Setup main menu
    SetupMainMenu(window);

    // Window event
    Bind(wxEVT_ACTIVATE_APP, &MainWindow::OnActivate, window);

    window->Maximize(true);
    window->Show();

    return true;
}
