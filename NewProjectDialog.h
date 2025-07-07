#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

//(*Headers(NewProjectDialog)
#include <wx/dialog.h>
#include <wx/filepicker.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/dir.h>

class NewProjectDialog: public wxDialog
{
    public:

        NewProjectDialog(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~NewProjectDialog();

        //(*Declarations(NewProjectDialog)
        wxDirPickerCtrl* locationInput;
        wxPanel* Panel2;
        wxStaticText* StaticText1;
        wxStaticText* StaticText2;
        wxTextCtrl* nameInput;
        //*)

    protected:

        //(*Identifiers(NewProjectDialog)
        static const wxWindowID ID_STATICTEXT1;
        static const wxWindowID ID_TEXTCTRL2;
        static const wxWindowID ID_STATICTEXT2;
        static const wxWindowID ID_DIRPICKERCTRL1;
        static const wxWindowID ID_PANEL2;
        //*)

    private:

        //(*Handlers(NewProjectDialog)
        //*)

        DECLARE_EVENT_TABLE()

        void OnCreate(wxCommandEvent& event);
};

#endif
