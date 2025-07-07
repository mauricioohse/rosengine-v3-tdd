#include "main_game_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"
#include "pause_scene.h"
#include "../core/input.h"
#include "sandfall.h"


MainGameScene g_mainGame;

void MainGameScene::OnLoad() 
{
    EntityManager* EM = &g_Engine.entityManager;
    
    EntityID box = RegisterEntity();
    EM->AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE | COMPONENT_WASD_CONTROLLER);
    g_Engine.componentArrays.Transforms[box].Init(300, 300);
    g_Engine.componentArrays.Sprites[box].Init(ResourceManager::GetTexture(TEXTURE_BOX));
    g_Engine.componentArrays.wasdControllers[box].Init(600);

    VP::InitViewPort();


    state = SceneState::INACTIVE;
}

void MainGameScene::handle_pause_input()
{
    static bool escapeReleased = false;
    
    if (!Input::IsKeyDown(SDL_SCANCODE_ESCAPE) && !escapeReleased) {
        escapeReleased = true;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_ESCAPE) && escapeReleased ) {
        g_pauseScene.state = SceneState::ACTIVE;
        g_mainGame.state = SceneState::INACTIVE;
        printf("main game scene deactivated\n");
        escapeReleased = false;
    }
}

void MainGameScene::OptionalUpdate(float deltaTime)
{
    handle_pause_input();


    VP::UpdateGrid();

    VP::DrawViewPort();

    VP::HandleInput();
}
