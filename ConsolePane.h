#ifndef CONSOLEPANE_H
#define CONSOLEPANE_H

//(*Headers(ConsolePane)
#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/sizer.h>
//*)

class ConsolePane: public wxPanel
{
    public:

        ConsolePane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~ConsolePane();

        //(*Declarations(ConsolePane)
        wxRichTextCtrl* RichTextCtrl1;
        //*)

    protected:

        //(*Identifiers(ConsolePane)
        static const wxWindowID ID_RICHTEXTCTRL1;
        //*)

    private:

        //(*Handlers(ConsolePane)
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
