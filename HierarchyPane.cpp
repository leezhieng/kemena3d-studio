#include "HierarchyPane.h"

//(*InternalHeaders(HierarchyPane)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(HierarchyPane)
const wxWindowID HierarchyPane::ID_BITMAPBUTTON1 = wxNewId();
const wxWindowID HierarchyPane::ID_SEARCHCTRL1 = wxNewId();
const wxWindowID HierarchyPane::ID_TREECTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(HierarchyPane,wxPanel)
    //(*EventTable(HierarchyPane)
    //*)
END_EVENT_TABLE()

HierarchyPane::HierarchyPane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(HierarchyPane)
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;

    Create(parent, wxID_ANY, wxDefaultPosition, wxSize(528,469), wxTAB_TRAVERSAL, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    FlexGridSizer1 = new wxFlexGridSizer(99, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer2->AddGrowableCol(1);
    FlexGridSizer2->AddGrowableRow(0);
    hierarchyAddButton = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxSize(25,25), wxBORDER_NONE, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    FlexGridSizer2->Add(hierarchyAddButton, 1, wxTOP|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SearchCtrl1 = new wxSearchCtrl(this, ID_SEARCHCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL1"));
    FlexGridSizer2->Add(SearchCtrl1, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
    sceneList = new wxTreeCtrl(this, ID_TREECTRL1, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE, wxDefaultValidator, _T("ID_TREECTRL1"));
    FlexGridSizer1->Add(sceneList, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    Layout();
    //*)

    Enable(false);

    setupStandardLabelIcons();

    hierarchyAddButton->Bind(wxEVT_BUTTON, &MainWindow::OnHierarchyAddButtonClicked, (MainWindow*)parent);
}

HierarchyPane::~HierarchyPane()
{
    //(*Destroy(HierarchyPane)
    //*)
}

void HierarchyPane::setupStandardLabelIcons()
{
    standardLabelIconList = new wxImageList(16, 16); // Size of the icons
    sceneList->AssignImageList(standardLabelIconList);

    wxBitmap worldBitmap("ICON_WORLD_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = worldBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexWorld = standardLabelIconList->Add(rescaled);
    }

    wxBitmap sceneBitmap("ICON_SCENE_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = sceneBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexScene = standardLabelIconList->Add(rescaled);
    }

    wxBitmap objectBitmap("ICON_EMPTY_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = objectBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexObject = standardLabelIconList->Add(rescaled);
    }

    wxBitmap meshBitmap("ICON_MESH_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = meshBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexMesh = standardLabelIconList->Add(rescaled);
    }

    wxBitmap lightBitmap("ICON_LIGHT_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = lightBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexLight = standardLabelIconList->Add(rescaled);
    }

    wxBitmap cameraBitmap("ICON_CAMERA_LABEL", wxBITMAP_TYPE_PNG_RESOURCE);
    {
        wxImage img = cameraBitmap.ConvertToImage();
        img.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap rescaled(img);
        iconIndexCamera = standardLabelIconList->Add(rescaled);
    }
}
