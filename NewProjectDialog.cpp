#include "NewProjectDialog.h"

//(*InternalHeaders(NewProjectDialog)
#include <wx/button.h>
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(NewProjectDialog)
const wxWindowID NewProjectDialog::ID_STATICTEXT1 = wxNewId();
const wxWindowID NewProjectDialog::ID_TEXTCTRL2 = wxNewId();
const wxWindowID NewProjectDialog::ID_STATICTEXT2 = wxNewId();
const wxWindowID NewProjectDialog::ID_DIRPICKERCTRL1 = wxNewId();
const wxWindowID NewProjectDialog::ID_PANEL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(NewProjectDialog,wxDialog)
    //(*EventTable(NewProjectDialog)
    //*)
END_EVENT_TABLE()

NewProjectDialog::NewProjectDialog(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(NewProjectDialog)
    wxBoxSizer* BoxSizer1;
    wxBoxSizer* BoxSizer2;
    wxGridBagSizer* GridBagSizer1;
    wxGridBagSizer* GridBagSizer2;
    wxGridBagSizer* GridBagSizer3;
    wxStdDialogButtonSizer* actionButtons;

    Create(parent, wxID_ANY, _("New Project"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    GridBagSizer3 = new wxGridBagSizer(0, 0);
    GridBagSizer1 = new wxGridBagSizer(-10, 0);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT1, _("Project Name"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    GridBagSizer1->Add(StaticText2, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
    nameInput = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(250,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    GridBagSizer1->Add(nameInput, wxGBPosition(1, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
    GridBagSizer3->Add(GridBagSizer1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND, 0);
    GridBagSizer2 = new wxGridBagSizer(-15, 0);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT2, _("Location"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    GridBagSizer2->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALL|wxEXPAND, 5);
    locationInput = new wxDirPickerCtrl(this, ID_DIRPICKERCTRL1, wxEmptyString, wxEmptyString, wxDefaultPosition, wxSize(250,-1), wxDIRP_DIR_MUST_EXIST|wxDIRP_USE_TEXTCTRL, wxDefaultValidator, _T("ID_DIRPICKERCTRL1"));
    GridBagSizer2->Add(locationInput, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    GridBagSizer3->Add(GridBagSizer2, wxGBPosition(1, 0), wxDefaultSpan, wxBOTTOM|wxEXPAND, 10);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    Panel2 = new wxPanel(this, ID_PANEL2, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL2"));
    Panel2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    actionButtons = new wxStdDialogButtonSizer();
    actionButtons->AddButton(new wxButton(Panel2, wxID_CANCEL, wxEmptyString));
    actionButtons->AddButton(new wxButton(Panel2, wxID_SAVE, _("Create")));
    actionButtons->Realize();
    BoxSizer2->Add(actionButtons, 1, wxTOP|wxBOTTOM, 10);
    Panel2->SetSizer(BoxSizer2);
    BoxSizer1->Add(Panel2, 1, wxALL|wxEXPAND, 0);
    GridBagSizer3->Add(BoxSizer1, wxGBPosition(2, 0), wxDefaultSpan, wxALL|wxEXPAND, 0);
    SetSizer(GridBagSizer3);
    GridBagSizer3->SetSizeHints(this);
    Center();
    //*)

    wxButton* createButton = dynamic_cast<wxButton*>(FindWindow(wxID_SAVE));
    if (createButton)
        createButton->Bind(wxEVT_BUTTON, &NewProjectDialog::OnCreate, this);
}

NewProjectDialog::~NewProjectDialog()
{
    //(*Destroy(NewProjectDialog)
    //*)
}

void NewProjectDialog::OnCreate(wxCommandEvent& event)
{
    wxString name = nameInput->GetValue();
    wxString path = locationInput->GetPath();

    if (name.IsEmpty())
    {
        wxMessageBox("Name cannot be empty!", "Input Error", wxOK | wxICON_WARNING);
        return;
    }

    wxFileName dir(path);
    if (!dir.DirExists())
    {
        wxMessageBox(wxString::Format("Directory does not exist:\n%s", path), "Invalid Directory",wxOK | wxICON_WARNING);
        return;
    }

    // Create project directory
    wxString fullPath = path + wxFILE_SEP_PATH + name;

    if (wxDir::Exists(fullPath))
    {
        wxMessageBox(wxString::Format("Folder already exists:\n%s", fullPath), "Warning",wxOK | wxICON_WARNING);
        return;
    }
    else
    {
        // Try to create the folder
        if (wxMkdir(fullPath))
        {
            // Create required folders
        }
        else
        {
            // Show error message if the folder creation fails
            wxMessageBox(wxString::Format("Failed to create folder: %s", fullPath), "Error", wxOK | wxICON_ERROR);
        }
    }

    EndModal(wxID_SAVE);
}
