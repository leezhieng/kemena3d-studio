#ifndef SCRIPTPANE_H
#define SCRIPTPANE_H

//(*Headers(ScriptPane)
#include <wx/button.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
//*)

#include <wx/tokenzr.h>
#include <wx/stc/stc.h> // wxStyledTextCtrl
#include <wx/file.h>
#include <wx/log.h>
#include <wx/msgdlg.h>

#include <vector>

//#include "kemena/kscriptmanager.h"

//using namespace kemena;

class ScriptPane: public wxPanel
{
    public:

        ScriptPane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ScriptPane();

        //(*Declarations(ScriptPane)
        wxButton* saveButton;
        wxStaticText* fileName;
        //*)

        wxStyledTextCtrl* editor;

        void openScript(wxString name, wxString fullPath);
        void refreshScriptName();
        void unsave();
        void save();

    protected:

        //(*Identifiers(ScriptPane)
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_BUTTON2;
        //*)

    private:

        //(*Handlers(ScriptPane)
        //*)

        DECLARE_EVENT_TABLE()

        bool scriptSaved = true;
        wxString scriptName = "";
        wxString scriptFullPath = "";

        //kScriptManager* scriptManager;
};

#endif
