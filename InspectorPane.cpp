#include "InspectorPane.h"

//(*InternalHeaders(InspectorPane)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(InspectorPane)
//*)

BEGIN_EVENT_TABLE(InspectorPane,wxPanel)
    //(*EventTable(InspectorPane)
    //*)
END_EVENT_TABLE()

InspectorPane::InspectorPane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(InspectorPane)
    Create(parent, id, wxDefaultPosition, wxSize(493,435), wxTAB_TRAVERSAL, _T("id"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    //*)

    Enable(false);
}

InspectorPane::~InspectorPane()
{
    //(*Destroy(InspectorPane)
    //*)
}

