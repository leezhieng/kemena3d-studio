#ifndef ARTPROVIDER_H
#define ARTPROVIDER_H

#include <wx/aui/aui.h>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/aui/framemanager.h>

class ArtProvider : public wxAuiDefaultDockArt
{
    public:
        ArtProvider();
        virtual ~ArtProvider();

        virtual void DrawPaneButton(wxDC &dc, wxWindow *window, int button, int buttonState, const wxRect &rect, wxAuiPaneInfo &pane);

    private:
        wxBitmap closeButton;
        wxBitmap pinButton;
        wxBitmap maximizeButton;

};

#endif // ARTPROVIDER_H
