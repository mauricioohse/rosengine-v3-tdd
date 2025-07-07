#include "pause_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "../core/input.h"
#include "main_game_scene.h"
#include "menu_scene.h"
#include "game.h"
#include "../core/play_sound.h"

PauseScene g_pauseScene;

static void ResumeButtonClicked() {
    g_pauseScene.state = SceneState::INACTIVE;
    g_mainGame.state = SceneState::ACTIVE;
    g_pauseScene.escapeReleased = false;
}

static void MainMenuButtonClicked() {
    g_pauseScene.state = SceneState::INACTIVE;
    g_menu.state = SceneState::ACTIVE;
    g_pauseScene.escapeReleased = false;
}

static void ToggleMusicButtonClicked() {
    g_Game.musicSystem.ToggleMusic();
    printf("music toggled\n");
}

static void ToggleSoundButtonClicked() {
    PlaySound::ToggleSound();
}

void PauseScene::OnLoad()
{

    escapeReleased = false;
    EntityManager* EM = &g_Engine.entityManager;


    // Create "PAUSED" text
    EntityID pausedText = RegisterEntity();
    ADD_TRANSFORM(pausedText, g_Engine.window->width / 2, g_Engine.window->height / 4, 0, 1.5f);
    ADD_TEXT(pausedText, ResourceManager::GetFont(FONT_FPS), "PAUSED");

    float buttonWidth = 200;
    float buttonHeight = 50;
    float buttonX = (g_Engine.window->width - buttonWidth) / 2;  // Center horizontally
    float buttonY = g_Engine.window->height / 2;                 // Place below title

    // Create Resume button
    EntityID resumeButton = RegisterEntity();
    EM->AddComponentToEntity(resumeButton,  COMPONENT_UIBOX );
    ADD_TRANSFORM(resumeButton, buttonX, buttonY, 0, 1.5f);
    ADD_TEXT(resumeButton, ResourceManager::GetFont(FONT_FPS), "resume");
    g_Engine.componentArrays.UIBoxs[resumeButton].Init(
        buttonWidth,
        buttonHeight,
        SDL_Color{50, 50, 50, 255},      // Dark gray background
        SDL_Color{200, 200, 200, 255},   // Light gray border
        2.0f, 
        ResumeButtonClicked
        );

    // Create Main Menu button
    EntityID mainMenuButton = RegisterEntity();
    EM->AddComponentToEntity(mainMenuButton, COMPONENT_UIBOX);
    ADD_TRANSFORM(mainMenuButton, buttonX, buttonY + buttonHeight + 20, 0, 1.5f);
    ADD_TEXT(mainMenuButton, ResourceManager::GetFont(FONT_FPS), "Main Menu");
    g_Engine.componentArrays.UIBoxs[mainMenuButton].Init(
        buttonWidth,
        buttonHeight,
        SDL_Color{50, 50, 50, 255},
        SDL_Color{200, 200, 200, 255},
        2.0f,
        MainMenuButtonClicked
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

    state = SceneState::INACTIVE;
} 

void PauseScene::handle_pause_input()
{
    
    if (!Input::IsKeyDown(SDL_SCANCODE_ESCAPE) && !escapeReleased) {
        escapeReleased = true;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_ESCAPE) && escapeReleased) {
        g_pauseScene.state = SceneState::INACTIVE;
        g_mainGame.state = SceneState::ACTIVE;
        printf("pause scene deactivated\n");
        escapeReleased = false;
    }
}

void PauseScene::OptionalUpdate(float deltaTime) {

    handle_pause_input();

}
