#include "menu_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"
#include "main_game_scene.h"
#include "../core/play_sound.h"

MenuScene g_menu;   

// Callback function
static void StartGameButtonClicked() {
    g_menu.state =  SceneState::INACTIVE;
    g_mainGame.state = SceneState::ACTIVE;
    snprintf(g_mainGame.levelPath, sizeof(g_mainGame.levelPath), "lvl1.csv");
    g_mainGame.OnLoad();
}

static void StartDebugLevelCallback()
{
    printf("StartDebugLevelCallback called \n");
    g_menu.state =  SceneState::INACTIVE;
    g_mainGame.state = SceneState::ACTIVE;
    snprintf(g_mainGame.levelPath, sizeof(g_mainGame.levelPath), "lvl2.csv");

    g_mainGame.OnLoad();
}

static void ToggleMusicButtonClicked() {
    g_Game.musicSystem.ToggleMusic();
    printf("music toggled\n");
}

static void ToggleSoundButtonClicked() {
    PlaySound::ToggleSound();
}

void MenuScene::OnLoad()
{
    EntityManager* EM = &g_Engine.entityManager;

    // Create centered game title
    EntityID titleText = RegisterEntity();
    ADD_TRANSFORM(titleText, g_Engine.window->width / 2, g_Engine.window->height / 3, 0, 1);
    ADD_TEXT(titleText, ResourceManager::GetFont(FONT_FPS), "Game Name");

    // Create Start Game button
    EntityID startButton = RegisterEntity();
    EM->AddComponentToEntity(startButton, COMPONENT_TRANSFORM | COMPONENT_UIBOX | COMPONENT_TEXT);
    
    // Initialize button position and size
    float buttonWidth = 200;
    float buttonHeight = 50;
    float buttonX = (g_Engine.window->width - buttonWidth) / 2;  // Center horizontally
    float buttonY = g_Engine.window->height / 2;                 // Place below title
    
    // Setup transform
    g_Engine.componentArrays.Transforms[startButton].Init(buttonX, buttonY);
    
    // Setup UI box with callback
    g_Engine.componentArrays.UIBoxs[startButton].Init(
        buttonWidth, 
        buttonHeight,
        SDL_Color{50, 50, 50, 255},      // Dark gray background
        SDL_Color{200, 200, 200, 255},   // Light gray border
        2.0f,                            // Border width
        StartGameButtonClicked           // Click callback
    );

    EntityID debugLevelButton = RegisterEntity();
    ADD_TRANSFORM(debugLevelButton, g_Engine.window->width*.8f - buttonWidth/2, g_Engine.window->height / 2, 0, 1.0f);
    ADD_UIBOX(debugLevelButton,50,50);
    g_Engine.componentArrays.UIBoxs[debugLevelButton].Init(
        50, 
        50,
        SDL_Color{50, 50, 50, 255},      // Dark gray background
        SDL_Color{200, 200, 200, 255},   // Light gray border
        2.0f,                            // Border width
        StartDebugLevelCallback          // Click callback
    );
    ADD_TEXT(debugLevelButton, ResourceManager::GetFont(FONT_FPS), "DEBUG LEVEL");
    
    // Setup text
    g_Engine.componentArrays.Texts[startButton].Init(
        ResourceManager::GetFont(FONT_FPS),
        "Start Game",
        TEXT_CENTER
    );

        // Create Toggle Music button
    EntityID toggleMusicButton = RegisterEntity();
    EM->AddComponentToEntity(toggleMusicButton, COMPONENT_UIBOX);
    ADD_TRANSFORM(toggleMusicButton, buttonX, buttonY + 2*buttonHeight + 40, 0, 1.5f);
    ADD_TEXT(toggleMusicButton, ResourceManager::GetFont(FONT_FPS), "Toggle Music");    
    g_Engine.componentArrays.UIBoxs[toggleMusicButton].Init(
        buttonWidth,
        buttonHeight,
        SDL_Color{50, 50, 50, 255},
        SDL_Color{200, 200, 200, 255},
        2.0f,
        ToggleMusicButtonClicked
    );

    // Create Toggle Sound button
    EntityID toggleSoundButton = RegisterEntity();
    EM->AddComponentToEntity(toggleSoundButton, COMPONENT_UIBOX);
    ADD_TRANSFORM(toggleSoundButton, buttonX, buttonY + 3*buttonHeight + 60, 0, 1.5f);
    ADD_TEXT(toggleSoundButton, ResourceManager::GetFont(FONT_FPS), "Toggle SFX");
    g_Engine.componentArrays.UIBoxs[toggleSoundButton].Init(
        buttonWidth,
        buttonHeight,
        SDL_Color{50, 50, 50, 255},
        SDL_Color{200, 200, 200, 255},
        2.0f,
        ToggleSoundButtonClicked
    );

    state = SceneState::ACTIVE;
}


