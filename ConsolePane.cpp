#include "ConsolePane.h"

//(*InternalHeaders(ConsolePane)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(ConsolePane)
const wxWindowID ConsolePane::ID_RICHTEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConsolePane,wxPanel)
    //(*EventTable(ConsolePane)
    //*)
END_EVENT_TABLE()

ConsolePane::ConsolePane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(ConsolePane)
    wxBoxSizer* BoxSizer1;

    Create(parent, id, wxDefaultPosition, wxSize(934,213), wxTAB_TRAVERSAL, _T("id"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    RichTextCtrl1 = new wxRichTextCtrl(this, ID_RICHTEXTCTRL1, _("Kemena Game Studio started"), wxDefaultPosition, wxDefaultSize, wxRE_MULTILINE|wxRE_READONLY|wxVSCROLL, wxDefaultValidator, _T("ID_RICHTEXTCTRL1"));
    wxRichTextAttr rchtxtAttr_1;
    rchtxtAttr_1.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ALIGN_LEFT);
    BoxSizer1->Add(RichTextCtrl1, 1, wxALL|wxEXPAND, 5);
    SetSizer(BoxSizer1);
    Layout();
    //*)

    Enable(false);
}

ConsolePane::~ConsolePane()
{
    //(*Destroy(ConsolePane)
    //*)
}

