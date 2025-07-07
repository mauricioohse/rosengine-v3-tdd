#pragma once
#include "../core/scene_manager.h"

class PauseScene : public SceneBase {
public:
    void OnLoad() override;
    void OptionalUpdate(float deltaTime) override;

    void handle_pause_input();

    
    bool escapeReleased;
};

extern PauseScene g_pauseScene; 