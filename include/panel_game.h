#ifndef PANEL_GAME_H
#define PANEL_GAME_H

#include "kemena/kemena.h"
#include <kemena/koffscreenrenderer.h>
#include "manager.h"

#include <GL/glew.h>
#include <imgui.h>
#include <vector>

using namespace kemena;

class Manager;

enum class GamePlayState { Stopped, Playing, Paused };

struct ObjectTransformSnapshot
{
    kString uuid;
    kVec3   pos;
    kQuat   rot;
    kVec3   scale;
    bool    active;
};

class PanelGame
{
public:
    PanelGame(kGuiManager* gui, Manager* manager);
    ~PanelGame();

    void draw(bool& isOpened);

    GamePlayState getPlayState() const { return playState; }

    // Returns 0 when paused so physics/animations freeze, real dt otherwise
    float getEffectiveDeltaTime(float dt) const;

    void pressPlay();
    void pressPause();
    void pressStop();

    Manager*     manager;
    kGuiManager* gui;

private:
    GamePlayState playState = GamePlayState::Stopped;
    std::vector<ObjectTransformSnapshot> sceneSnapshot;

    kOffscreenRenderer* gameRenderer = nullptr;
    int lastRendererW = 0;
    int lastRendererH = 0;

    // Returns the camera to use for game rendering (nullptr = none → black screen)
    kCamera* findGameCamera() const;

    void captureSnapshot();
    void restoreSnapshot();
    void captureNodeRecursive(kObject* node);
};

#endif // PANEL_GAME_H
