#include "ScenePane.h"

//(*InternalHeaders(ScenePane)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(ScenePane)
const wxWindowID ScenePane::ID_BITMAPBUTTON2 = wxNewId();
const wxWindowID ScenePane::ID_BITMAPBUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ScenePane,wxPanel)
    //(*EventTable(ScenePane)
    //*)
END_EVENT_TABLE()

ScenePane::ScenePane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(ScenePane)
    wxBoxSizer* canvasSizer;
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;

    Create(parent, wxID_ANY, wxDefaultPosition, wxSize(653,451), 0, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    FlexGridSizer1 = new wxFlexGridSizer(3, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    FlexGridSizer2 = new wxFlexGridSizer(1, 99, 0, -10);
    FlexGridSizer2->AddGrowableCol(0);
    FlexGridSizer2->AddGrowableCol(1);
    scenePlayButton = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxNullBitmap, wxDefaultPosition, wxSize(40,30), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    FlexGridSizer2->Add(scenePlayButton, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5);
    sceneSettingsButton = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxSize(40,30), wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    FlexGridSizer2->Add(sceneSettingsButton, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 0);
    canvasSizer = new wxBoxSizer(wxHORIZONTAL);
    FlexGridSizer1->Add(canvasSizer, 1, wxALL|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    Layout();
    //*)

    Enable(false);

    // Setup OpenGL context
    wxGLContextAttrs cxtAttrs;
    cxtAttrs.PlatformDefaults()
                .OGLVersion(4, 5)
                .CoreProfile()
                .ForwardCompatible()
                .EndList();

    wxGLAttributes attrs;
    attrs.PlatformDefaults().Defaults().RGBA().DoubleBuffer().Depth(24).Stencil(8).EndList();

    renderer = new RendererWindow(this, cxtAttrs, attrs, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    canvasSizer->Add(renderer, 1, wxEXPAND);
}

ScenePane::~ScenePane()
{
    //(*Destroy(ScenePane)
    //*)
}

RendererWindow* ScenePane::getRendererWindow()
{
    return renderer;
}
