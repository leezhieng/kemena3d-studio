#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H

#include "kemena/kemena.h"
#include "manager.h"

using namespace kemena;

class SplashScreen
{
public:
    SplashScreen(kGuiManager* gui, kAssetManager* assetManager, Manager* manager);
    bool draw();
    bool isOpen() const { return open; }
    void show()         { open = true; }

private:
    kGuiManager*  gui;
    Manager*      manager;
    ImTextureRef  texSplash   = nullptr;
    ImTextureRef  texLogo     = nullptr;
    unsigned int  splashGlId  = 0;
    unsigned int  logoGlId    = 0;
    bool          open        = true;
};

#endif // SPLASH_SCREEN_H
