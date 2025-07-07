#ifndef SCENEPANE_H
#define SCENEPANE_H

#include "RendererWindow.h"

//(*Headers(ScenePane)
#include <wx/bmpbuttn.h>
#include <wx/panel.h>
#include <wx/sizer.h>
//*)

#include <wx/glcanvas.h>

class ScenePane: public wxPanel
{
    public:

        ScenePane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ScenePane();

        //(*Declarations(ScenePane)
        wxBitmapButton* scenePlayButton;
        wxBitmapButton* sceneSettingsButton;
        //*)

        RendererWindow* getRendererWindow();

    protected:

        //(*Identifiers(ScenePane)
        static const wxWindowID ID_BITMAPBUTTON2;
        static const wxWindowID ID_BITMAPBUTTON1;
        //*)

    private:

        //(*Handlers(ScenePane)
        //*)

        DECLARE_EVENT_TABLE()

        RendererWindow* renderer;
};

#endif
