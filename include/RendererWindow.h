#ifndef RENDERERWINDOW_H
#define RENDERERWINDOW_H

#include "kemena/kemena.h"

//#define GLEW_STATIC
#include <GL/glew.h>

#include <wx/glcanvas.h>
#include <wx/dcclient.h>

// Platform-specific includes
#ifdef _WIN32
    #include <wx/msw/private.h>
    #include <windows.h>
#elif defined(__linux__)
    #include <gdk/gdkx.h>
    #include <gtk/gtk.h>
    #include <gdk/gdk.h>
#endif

using namespace kemena;

class RendererWindow : public wxGLCanvas
{
    public:
        RendererWindow(wxWindow* parent,
                const wxGLContextAttrs& contextAttrs,
                const wxGLAttributes& dispAttrs,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size);
        virtual ~RendererWindow();

        void OnPaint(wxPaintEvent& event);
        void Resize(wxSizeEvent& event);

        kWorld* getWorld();
        void newScene();

    protected:

    private:
        wxGLContext* m_glContext;

        // Kemena3D
        kRenderer* renderer;
        kAssetManager* assetManager;
        kWorld* world;

        kScene* editorScene;
        kCamera* editorCamera;
};

#endif // RENDERERWINDOW_H
