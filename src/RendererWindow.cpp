#include "RendererWindow.h"

RendererWindow::RendererWindow(wxWindow* parent,
                const wxGLContextAttrs& contextAttrs,
                const wxGLAttributes& dispAttrs,
                wxWindowID id,
                const wxPoint& pos,
                const wxSize& size) :
    wxGLCanvas(parent, dispAttrs, id, pos, size),
    m_glContext(new wxGLContext(this, nullptr, &contextAttrs))
{
    SetCurrent(*m_glContext);
    Bind(wxEVT_PAINT, &RendererWindow::OnPaint, this);

    void* nativeHandle = nullptr;

    #ifdef _WIN32
        HWND hwnd = static_cast<HWND>(this->GetHandle());
        nativeHandle = (void*)hwnd;
    #elif defined(__linux__)
        // Cast the wxWindow handle to GtkWidget* (wxWidgets GTK backend)
        GtkWidget* gtkWidget = static_cast<GtkWidget*>(this->GetHandle());
        if (gtkWidget && gtk_widget_get_window(gtkWidget))
        {
            GdkWindow* gdkWin = gtk_widget_get_window(gtkWidget);
            nativeHandle = (void*)(uintptr_t)GDK_WINDOW_XID(gdkWin);
        }
        else
        {
            std::cerr << "Failed to retrieve GDK window from wxWidgets handle." << std::endl;
        }
    #endif

    if (!nativeHandle)
    {
        std::cerr << "Failed to retrieve native window handle!" << std::endl;
    }

    kWindow* window = createWindow(800, 600, "Demo", nativeHandle);
    renderer = createRenderer(window);
    renderer->setClearColor(glm::vec4(0.4f, 0.6f, 0.8f, 1.0f));

    assetManager = createAssetManager();
    world = createWorld(assetManager);

    // Create editor scene, which will not be shown on the hierarchy
    // Gizmo and editor camera will be staying in this scene
    editorScene = world->createScene("Editor");

    // Create default editor camera
    editorCamera = editorScene->addCamera(glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    editorCamera->setName("Editor");


    // Create a default scene
    editorScene = world->createScene("Scene");

    // Test
    kCamera* camera = editorScene->addCamera(glm::vec3(-25.0f, 23.0f, 25.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    camera->setName("Camera");
    kLight* light_sun = editorScene->addSunLight(glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.5f, -1.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f));
    light_sun->setName("Sun Light");
    //kMesh* sphere2 = scene->addMesh("D:/Projects/kemena-engine/Assets/racoon/Racoon_Anim.fbx");
    //sphere2->setName("Mesh");

}

RendererWindow::~RendererWindow()
{
}

void RendererWindow::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    //renderer->clear();

    wxSize size = GetSize();

    //if (editorScene != nullptr)
        //renderer->render(editorScene, 0, 0, size.GetWidth(), size.GetHeight(), 0.0f);

    SwapBuffers();
}

void RendererWindow::Resize(wxSizeEvent& event)
{
}

kWorld* RendererWindow::getWorld()
{
    return world;
}

void RendererWindow::newScene()
{
    kScene* scene = world->createScene("Scene");
}
