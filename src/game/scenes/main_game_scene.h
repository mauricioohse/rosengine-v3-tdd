#pragma once
#include "../core/scene_manager.h"

// level definitions
#define LEVEL_ONE 1
#define LEVEL_DEBUG 99

struct MainGameScene : public SceneBase {
    void OnLoad() override;
    void OptionalUpdate(float deltaTime) override;

    void handle_pause_input();
    char levelPath[30] = "lvl1.csv";
    int level = 1;

};

extern MainGameScene g_mainGame;
