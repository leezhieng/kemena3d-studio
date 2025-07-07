#ifndef INSPECTORPANE_H
#define INSPECTORPANE_H

//(*Headers(InspectorPane)
#include <wx/panel.h>
//*)

class InspectorPane: public wxPanel
{
    public:

        InspectorPane(wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
        virtual ~InspectorPane();

        //(*Declarations(InspectorPane)
        //*)

    protected:

        //(*Identifiers(InspectorPane)
        //*)

    private:

        //(*Handlers(InspectorPane)
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
