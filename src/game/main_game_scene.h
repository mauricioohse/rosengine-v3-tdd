#pragma once
#include "../core/scene_manager.h"

class MainGameScene : public SceneBase {
public:
    void OnLoad() override;
    void OptionalUpdate(float deltaTime) override;

    void handle_pause_input();
};

extern MainGameScene g_mainGame;
