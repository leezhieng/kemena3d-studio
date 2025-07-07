#ifndef PROJECTPANE_H
#define PROJECTPANE_H

//(*Headers(ProjectPane)
#include <wx/bmpbuttn.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
//*)

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/log.h>

#include "MainWindow.h"
#include "ScriptPane.h"

class MainWindow;

class ProjectPane: public wxPanel
{
    public:

        ProjectPane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ProjectPane();

        //(*Declarations(ProjectPane)
        wxBitmapButton* projectAddButton;
        wxBitmapButton* projectFolderUpButton;
        wxListCtrl* projectList;
        wxSearchCtrl* SearchCtrl;
        //*)

        void setMainWindow(MainWindow* newWindow);

        void displayDirectory(wxString path);
        void setupStandardFileIcons();

        void OnItemDoubleClicked(wxListEvent& event);
        void OnFolderUpButtonClicked(wxCommandEvent& event);

    protected:

        //(*Identifiers(ProjectPane)
        static const wxWindowID ID_BITMAPBUTTON1;
        static const wxWindowID ID_BITMAPBUTTON2;
        static const wxWindowID ID_SEARCHCTRL;
        static const wxWindowID ID_LISTCTRL1;
        //*)

    private:

        //(*Handlers(ProjectPane)
        void OnBitmapButton1Click(wxCommandEvent& event);
        void OnBitmapButton1Click1(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()

        MainWindow* window;

        wxImageList* standardFileIconList;

        int iconIndexFolder;
        int iconIndexText;
        int iconIndexImage;
        int iconIndexScript;
        int iconIndexAudio;
        int iconIndexVideo;
        int iconIndexModel;
        int iconIndexPrefab;
        int iconIndexWorld;
        int iconIndexMaterial;
        int iconIndexOther;
};

#endif
