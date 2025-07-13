#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include "../core/resource_manager.h"
#include "../core/ecs/systems/render_system.h"
#include "../core/ecs/systems/wasd_controller_system.h"
#include "../core/ecs/systems/collision_system.h"
#include "../core/ecs/systems/camera_system.h"
#include "../core/ecs/systems/background_system.h"
#include "../core/ecs/systems/music_system.h"
#include "../core/ecs/systems/text_update_system.h"
#include "../core/ecs/systems/ui_system.h"
#include <math.h>
#include <algorithm>
#include "menu_scene.h"
#include "main_game_scene.h"
#include "pause_scene.h"
#include "enemy_movement_system.h"
#include "tower_behavior_system.h"
#include "projectile_system.h"
#include "move_to_target_system.h"
#include "life_time_system.h"

Game g_Game;

bool Game::Init() {
    g_Engine.systemManager.RegisterSystem(new RenderSystem());
    g_Engine.systemManager.RegisterSystem(new WASDControllerSystem());
    g_Engine.systemManager.RegisterSystem(new TextUpdateSystem());
    g_Engine.systemManager.RegisterSystem(new UISystem());
    g_Engine.systemManager.RegisterSystem(&musicSystem);
    g_Engine.systemManager.RegisterSystem(new enemy_movement_system());
    g_Engine.systemManager.RegisterSystem(new tower_behavior_system());
    g_Engine.systemManager.RegisterSystem(new projectile_system());
    g_Engine.systemManager.RegisterSystem(new move_to_target_system());
    g_Engine.systemManager.RegisterSystem(new life_time_system());
    


    g_Engine.sceneManager.PushScene(&g_menu);
    g_Engine.sceneManager.PushScene(&g_mainGame);
    g_Engine.sceneManager.PushScene(&g_pauseScene);

    g_menu.OnLoad();
    g_mainGame.OnLoad();
    g_pauseScene.OnLoad();
    

    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){

}

void Game::Update(float deltaTime) {

    g_Engine.sceneManager.Update(deltaTime);

}


void Game::Render() {
    //g_Engine.systemManager.UpdateSystems(g_Engine.deltaTime, g_Engine.entityManager.activeEntities, &g_Engine.componentArrays);



}

void Game::Cleanup() {
    
}

void Game::Reset() {

    gameState = GAME_STATE_START;

}


