#include "ArtProvider.h"

ArtProvider::ArtProvider()
{
    {
        wxBitmap bmp("ICON_CLOSE_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage image = bmp.ConvertToImage();
        image.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap resized(image);
        closeButton = resized;
    }

    {
        wxBitmap bmp("ICON_DETACH_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage image = bmp.ConvertToImage();
        image.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap resized(image);
        pinButton = resized;
    }

    {
        wxBitmap bmp("ICON_MAXIMIZE_BUTTON", wxBITMAP_TYPE_PNG_RESOURCE);
        wxImage image = bmp.ConvertToImage();
        image.Rescale(16, 16, wxIMAGE_QUALITY_HIGH);
        wxBitmap resized(image);
        maximizeButton = resized;
    }
}

ArtProvider::~ArtProvider()
{
}

void ArtProvider::DrawPaneButton(wxDC &dc, wxWindow *window, int button, int buttonState, const wxRect &rect, wxAuiPaneInfo &pane)
{
    switch (button)
    {
        case wxAUI_BUTTON_CLOSE:
        {
            // Calculate position to center the bitmap vertically and horizontally
            int x = rect.x + (rect.width - closeButton.GetWidth()) / 2;
            int y = rect.y + (rect.height - closeButton.GetHeight()) / 2;

            dc.DrawBitmap(closeButton, x, y, true);
            break;
        }
        case wxAUI_BUTTON_PIN:
        {
            // Calculate position to center the bitmap vertically and horizontally
            int x = rect.x + (rect.width - pinButton.GetWidth()) / 2;
            int y = rect.y + (rect.height - pinButton.GetHeight()) / 2;

            dc.DrawBitmap(pinButton, x, y, true);
            break;
        }
        case wxAUI_BUTTON_MAXIMIZE_RESTORE:
        {
            // Calculate position to center the bitmap vertically and horizontally
            int x = rect.x + (rect.width - maximizeButton.GetWidth()) / 2;
            int y = rect.y + (rect.height - maximizeButton.GetHeight()) / 2;

            dc.DrawBitmap(maximizeButton, x, y, true);
            break;
        }
        // Handle other button types as needed
        default:
            wxAuiDefaultDockArt::DrawPaneButton(dc, window, button, buttonState, rect, pane); // Default drawing
    }
}
