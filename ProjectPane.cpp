#include "ProjectPane.h"

//(*InternalHeaders(ProjectPane)
#include <wx/intl.h>
#include <wx/settings.h>
#include <wx/string.h>
//*)

//(*IdInit(ProjectPane)
const wxWindowID ProjectPane::ID_BITMAPBUTTON1 = wxNewId();
const wxWindowID ProjectPane::ID_BITMAPBUTTON2 = wxNewId();
const wxWindowID ProjectPane::ID_SEARCHCTRL = wxNewId();
const wxWindowID ProjectPane::ID_LISTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ProjectPane,wxPanel)
    //(*EventTable(ProjectPane)
    //*)
END_EVENT_TABLE()

ProjectPane::ProjectPane(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
    //(*Initialize(ProjectPane)
    wxFlexGridSizer* FlexGridSizer1;
    wxFlexGridSizer* FlexGridSizer2;

    Create(parent, wxID_ANY, wxDefaultPosition, wxSize(610,246), wxTAB_TRAVERSAL, _T("wxID_ANY"));
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
    FlexGridSizer1 = new wxFlexGridSizer(2, 1, 0, 0);
    FlexGridSizer1->AddGrowableCol(0);
    FlexGridSizer1->AddGrowableRow(1);
    FlexGridSizer2 = new wxFlexGridSizer(0, 3, 0, 0);
    FlexGridSizer2->AddGrowableCol(2);
    FlexGridSizer2->AddGrowableRow(0);
    projectFolderUpButton = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxNullBitmap, wxDefaultPosition, wxSize(25,25), wxBORDER_NONE, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
    FlexGridSizer2->Add(projectFolderUpButton, 1, wxTOP|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    projectAddButton = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxNullBitmap, wxDefaultPosition, wxSize(25,25), wxBORDER_NONE, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
    FlexGridSizer2->Add(projectAddButton, 1, wxTOP|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SearchCtrl = new wxSearchCtrl(this, ID_SEARCHCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_SEARCHCTRL"));
    FlexGridSizer2->Add(SearchCtrl, 1, wxTOP|wxLEFT|wxEXPAND, 5);
    FlexGridSizer1->Add(FlexGridSizer2, 1, wxALL|wxEXPAND, 5);
    projectList = new wxListCtrl(this, ID_LISTCTRL1, wxDefaultPosition, wxDefaultSize, wxLC_ICON|wxLC_AUTOARRANGE|wxLC_SINGLE_SEL|wxVSCROLL, wxDefaultValidator, _T("ID_LISTCTRL1"));
    FlexGridSizer1->Add(projectList, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND, 5);
    SetSizer(FlexGridSizer1);
    Layout();

    Connect(ID_BITMAPBUTTON1, wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&ProjectPane::OnBitmapButton1Click1);
    //*)

    Enable(false);

    setupStandardFileIcons();

    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectPane::OnItemDoubleClicked, this);
    projectFolderUpButton->Bind(wxEVT_BUTTON, &ProjectPane::OnFolderUpButtonClicked, this);
}

ProjectPane::~ProjectPane()
{
    //(*Destroy(ProjectPane)
    //*)
}

void ProjectPane::OnBitmapButton1Click1(wxCommandEvent& event)
{
}

void ProjectPane::setMainWindow(MainWindow* newWindow)
{
    window = newWindow;
}

void ProjectPane::displayDirectory(wxString path)
{
    // Clear previous items if needed
    projectList->DeleteAllItems();

    wxDir dir(path);
    if (!dir.IsOpened())
        return;

    wxString fileName;
    bool cont = dir.GetFirst(&fileName);
    while (cont)
    {
        wxString fullPath = path + wxFILE_SEP_PATH + fileName;
        wxFileName fileInfo(fullPath);

        if (wxDirExists(fullPath))
        {
            // Is folder
            projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexFolder);
        }
        else if (wxFileExists(fullPath))
        {
            // Is file
            wxString ext = fileInfo.GetExt().Lower();

            if (ext == "txt" || ext == "ini" || ext == "xml" || ext == "json")
            {
                // Text
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexText);
            }
            else if (ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "png" || ext == "gif" || ext == "tiff" || ext == "tga")
            {
                // Image
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexImage);
            }
            else if (ext == "as")
            {
                // Script
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexScript);
            }
            else if (ext == "mp3" || ext == "wav")
            {
                // Audio
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexAudio);
            }
            else if (ext == "mp4" || ext == "mov" || ext == "avi" || ext == "webm")
            {
                // Video
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexVideo);
            }
            else if (ext == "obj" || ext == "fbx" || ext == "gltf" || ext == "glb" || ext == "usd" || ext == "dae" || ext == "stl")
            {
                // Model
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexModel);
            }
            else if (ext == "world")
            {
                // World
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexWorld);
            }
            else if (ext == "pfb")
            {
                // Prefab
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexPrefab);
            }
            else if (ext == "mat")
            {
                // Material
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexMaterial);
            }
            else
            {
                // Other
                projectList->InsertItem(projectList->GetItemCount(), fileName, iconIndexOther);
            }
        }
        cont = dir.GetNext(&fileName);
    }
}

void ProjectPane::setupStandardFileIcons()
{
    standardFileIconList = new wxImageList(32, 32);
    projectList->AssignImageList(standardFileIconList, wxIMAGE_LIST_NORMAL);

    // Folder
    wxBitmap bitmapFolder("ICON_FOLDER_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapFolder.IsOk())
        wxLogError("Failed to load ICON_FOLDER_FILE");
    else
    {
        wxImage image = bitmapFolder.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexFolder = standardFileIconList->Add(resizedBitmap);
    }

    // Text
    wxBitmap bitmapText("ICON_TEXT_FILE", wxBITMAP_TYPE_ICO_RESOURCE);
    if (!bitmapText.IsOk())
        wxLogError("Failed to load ICON_TEXT_FILE");
    else
        iconIndexText = standardFileIconList->Add(bitmapText);

    // Image
    wxBitmap bitmapImage("ICON_IMAGE_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapImage.IsOk())
        wxLogError("Failed to load ICON_IMAGE_FILE");
    else
    {
        wxImage image = bitmapImage.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexImage = standardFileIconList->Add(resizedBitmap);
    }

    // Script
    wxBitmap bitmapScript("ICON_SCRIPT_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapScript.IsOk())
        wxLogError("Failed to load ICON_SCRIPT_FILE");
    else
    {
        wxImage image = bitmapScript.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexScript = standardFileIconList->Add(resizedBitmap);
    }

    // Audio
    wxBitmap bitmapAudio("ICON_AUDIO_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapAudio.IsOk())
        wxLogError("Failed to load ICON_AUDIO_FILE");
    else
    {
        wxImage image = bitmapAudio.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexAudio = standardFileIconList->Add(resizedBitmap);
    }

    // Video
    wxBitmap bitmapVideo("ICON_VIDEO_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapVideo.IsOk())
        wxLogError("Failed to load ICON_VIDEO_FILE");
    else
    {
        wxImage image = bitmapVideo.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexVideo = standardFileIconList->Add(resizedBitmap);
    }

    // Model
    wxBitmap bitmapModel("ICON_MODEL_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapModel.IsOk())
        wxLogError("Failed to load ICON_MODEL_FILE");
    else
    {
        wxImage image = bitmapModel.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexModel = standardFileIconList->Add(resizedBitmap);
    }

    // Prefab
    wxBitmap bitmapPrefab("ICON_PREFAB_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapPrefab.IsOk())
        wxLogError("Failed to load ICON_PREFAB_FILE");
    else
    {
        wxImage image = bitmapPrefab.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexPrefab = standardFileIconList->Add(resizedBitmap);
    }

    // World
    wxBitmap bitmapWorld("ICON_WORLD_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapWorld.IsOk())
        wxLogError("Failed to load ICON_SCENE_FILE");
    else
    {
        wxImage image = bitmapWorld.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexWorld = standardFileIconList->Add(resizedBitmap);
    }

    // Material
    wxBitmap bitmapMaterial("ICON_MATERIAL_FILE", wxBITMAP_TYPE_PNG_RESOURCE);
    if (!bitmapMaterial.IsOk())
        wxLogError("Failed to load ICON_SCENE_FILE");
    else
    {
        wxImage image = bitmapMaterial.ConvertToImage();
        image.Rescale(32, 32, wxIMAGE_QUALITY_HIGH);
        wxBitmap resizedBitmap(image);
        iconIndexMaterial = standardFileIconList->Add(resizedBitmap);
    }

    // Other
    wxBitmap bitmapOther("ICON_OTHER_FILE", wxBITMAP_TYPE_ICO_RESOURCE);
    if (!bitmapOther.IsOk())
        wxLogError("Failed to load ICON_OTHER_FILE");
    else
        iconIndexOther = standardFileIconList->Add(bitmapOther);
}

void ProjectPane::OnItemDoubleClicked(wxListEvent& event)
{
    long itemIndex = event.GetIndex();

    wxString name = projectList->GetItemText(itemIndex);
    wxString fullPath = wxString(window->getFileManager()->getCurrentDirPath()) + wxFILE_SEP_PATH + name;

    wxFileName fileInfo(fullPath);

    if (wxDirExists(fullPath))
    {
        // Is Folder
        window->openFolder(name);

        //std::cout << name << std::endl;
    }
    else if (wxFileExists(fullPath))
    {
        // Is file
        wxString ext = fileInfo.GetExt().Lower();

        if (ext == "txt" || ext == "ini" || ext == "xml" || ext == "json")
        {
            // Text
        }
        else if (ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "png" || ext == "gif" || ext == "tiff" || ext == "tga")
        {
            // Image
        }
        else if (ext == "as")
        {
            // Script
            wxAuiPaneInfo& paneInfo = window->AuiManager->GetPane("script");
            if (!paneInfo.IsOk())
            {
                wxLogError("Script pane not found.");
                return;
            }

            if (!paneInfo.IsShown())
            {
                paneInfo.Show();
                window->AuiManager->Update();
            }

            ScriptPane* pane = dynamic_cast<ScriptPane*>(paneInfo.window);
            if (!pane)
            {
                wxLogError("Pane has no associated window.");
                return;
            }

            pane->openScript(name, fullPath);
        }
        else if (ext == "mp3" || ext == "wav")
        {
            // Audio
        }
        else if (ext == "mp4" || ext == "mov" || ext == "avi" || ext == "webm")
        {
            // Video
        }
        else if (ext == "obj" || ext == "fbx" || ext == "gltf" || ext == "glb" || ext == "usd" || ext == "dae" || ext == "stl")
        {
            // Model
        }
        else if (ext == "world")
        {
            // World
        }
        else if (ext == "pfb")
        {
            // Prefab
        }
        else if (ext == "mat")
        {
            // Material
        }
        else
        {
            // Other
        }

    }
}

void ProjectPane::OnFolderUpButtonClicked(wxCommandEvent& event)
{
    window->closeFolder();
}
