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
    TransformComponent::Add(titleText, g_Engine.window->width / 2, g_Engine.window->height / 3, 0, 1);
    TextComponent::Add(titleText, ResourceManager::GetFont(FONT_FPS), "Game Name");

    // Initialize button position and size
    float buttonWidth = 200;
    float buttonHeight = 50;
    float buttonX = (g_Engine.window->width - buttonWidth) / 2;  // Center horizontally
    float buttonY = g_Engine.window->height / 2;                 // Place below title

    // Create Start Game button
    EntityID startButton = RegisterEntity();

    TransformComponent::Add(startButton, buttonX, buttonY);
    TextComponent::Add(startButton, ResourceManager::GetFont(FONT_FPS), "Start Game", TEXT_CENTER);
    UIBoxComponent::Add(startButton,
                        buttonWidth,
                        buttonHeight,
                        SDL_Color{50, 50, 50, 255},      // Dark gray background
                        SDL_Color{200, 200, 200, 255},   // Light gray border
                        2.0f,                            // Border width
                        StartGameButtonClicked);         // Click callback

    // Create Debug Level button
    EntityID debugLevelButton = RegisterEntity();

    TransformComponent::Add(debugLevelButton, g_Engine.window->width * 0.8f - buttonWidth / 2, g_Engine.window->height / 2, 0, 1.0f);
    TextComponent::Add(debugLevelButton, ResourceManager::GetFont(FONT_FPS), "DEBUG LEVEL");
    UIBoxComponent::Add(debugLevelButton,
                        50,
                        50,
                        SDL_Color{50, 50, 50, 255},      // Dark gray background
                        SDL_Color{200, 200, 200, 255},   // Light gray border
                        2.0f,                            // Border width
                        StartDebugLevelCallback);        // Click callback

    // Create Toggle Music button
    EntityID toggleMusicButton = RegisterEntity();

    TransformComponent::Add(toggleMusicButton, buttonX, buttonY + 2 * buttonHeight + 40, 0, 1.5f);
    TextComponent::Add(toggleMusicButton, ResourceManager::GetFont(FONT_FPS), "Toggle Music");
    UIBoxComponent::Add(toggleMusicButton,
                        buttonWidth,
                        buttonHeight,
                        SDL_Color{50, 50, 50, 255},      // Dark gray background
                        SDL_Color{200, 200, 200, 255},   // Light gray border
                        2.0f,                            // Border width
                        ToggleMusicButtonClicked);       // Click callback

    // Create Toggle Sound button
    EntityID toggleSoundButton = RegisterEntity();

    TransformComponent::Add(toggleSoundButton, buttonX, buttonY + 3 * buttonHeight + 60, 0, 1.5f);
    TextComponent::Add(toggleSoundButton, ResourceManager::GetFont(FONT_FPS), "Toggle SFX");
    UIBoxComponent::Add(toggleSoundButton,
                        buttonWidth,
                        buttonHeight,
                        SDL_Color{50, 50, 50, 255},      // Dark gray background
                        SDL_Color{200, 200, 200, 255},   // Light gray border
                        2.0f,                            // Border width
                        ToggleSoundButtonClicked);       // Click callback

    state = SceneState::ACTIVE;
}


