#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//(*Headers(MainWindow)
#include <wx/aui/aui.h>
#include <wx/frame.h>
//*)

#include <wx/log.h>
#include <wx/event.h>

#include <thread>

#include "FileManager.h"
#include "NewProjectDialog.h"
#include "ProjectPane.h"
#include "HierarchyPane.h"
#include "ScenePane.h"
//#include "RendererWindow.h"

//#include "kemena/kemena.h"
//#include "kemena/kworld.h"

//using namespace kemena;

class MainWindow: public wxFrame
{
    public:

        MainWindow(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~MainWindow();

        //(*Declarations(MainWindow)
        wxAuiManager* AuiManager;
        //*)

        void OnActivate(wxActivateEvent& event);
        void OnNewProject(wxCommandEvent& event);
        void OnOpenProject(wxCommandEvent& event);

        void OnHierarchyAddButtonClicked(wxCommandEvent& event);

        wxMenu* cloneMenu(wxMenu* sourceMenu);

        void setEnableAllPanes(bool enabled);
        void setEnableAllMenus(bool enabled);
        void loadSceneList();
        void refreshProjectList();

        void openFolder(wxString name);
        void closeFolder();

        void updateWindowTitle();

        void newWorld();
        void loadWorld(std::string fileName);

        FileManager* getFileManager();

        wxMenu* objectMenu;

    protected:

        //(*Identifiers(MainWindow)
        //*)

    private:

        //(*Handlers(MainWindow)
        //*)

        DECLARE_EVENT_TABLE()

        FileManager* fileManager;

        const wxString windowTitle = "Kemena3D Studio";
        wxString projectName = "";
        wxString worldName = "";
        bool projectSaved = true;
};

#endif
