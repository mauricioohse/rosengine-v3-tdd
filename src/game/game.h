#pragma once
#include "../core/window.h"
#include "../core/ecs/component_macros.h"
#include "game_systems/game_name_system.h"
#include "../core/ecs/systems/music_system.h"

// Add new game state
enum GameState {
    GAME_STATE_START,    // New state for start screen
    GAME_STATE_PLAYING,
    GAME_STATE_GAMEOVER,
    GAME_STATE_PAUSED
};


class Game {
public:
    bool Init();
    void HandleInput();
    void Update(float deltaTime);
    void Render();
    void Cleanup();
    void Reset();

    EntityID playerEntity;
    GameState gameState;
    MusicSystem musicSystem;
};

extern Game g_Game;  // Global game instance 
