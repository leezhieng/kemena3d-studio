#ifndef HIERARCHYPANE_H
#define HIERARCHYPANE_H

//(*Headers(HierarchyPane)
#include <wx/bmpbuttn.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/treectrl.h>
//*)

#include "MainWindow.h"

class HierarchyPane: public wxPanel
{
    public:

        HierarchyPane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~HierarchyPane();

        //(*Declarations(HierarchyPane)
        wxBitmapButton* hierarchyAddButton;
        wxSearchCtrl* SearchCtrl1;
        wxTreeCtrl* sceneList;
        //*)

        void setupStandardLabelIcons();

        int iconIndexWorld;
        int iconIndexScene;
        int iconIndexObject;
        int iconIndexMesh;
        int iconIndexPrefab;
        int iconIndexLight;
        int iconIndexCamera;

    protected:

        //(*Identifiers(HierarchyPane)
        static const wxWindowID ID_BITMAPBUTTON1;
        static const wxWindowID ID_SEARCHCTRL1;
        static const wxWindowID ID_TREECTRL1;
        //*)

    private:

        //(*Handlers(HierarchyPane)
        //*)

        DECLARE_EVENT_TABLE()

        wxImageList* standardLabelIconList;
};

#endif
