#include "main_game_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"
#include "pause_scene.h"
#include "../core/input.h"
#include "grid.h"
#include "tower_placement.h"
#include "general_systems.h"
#include "resolve_elements_system.h"
#include "enemy_spawner.h"
#include "wave_system.h"
#include "player_life_system.h"
#include <cstdlib>


MainGameScene g_mainGame;

static void loadDebugLevelTowers()
{
    // loads all towers types spaced horizontally
    // spawn one tower of each type, each in its own row, spaced by radius
    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH;
    int startY = Grid::GRID_START_POINT.y + Grid::GRID_SQUARE_LENGTH;
    int currentX = startX;

    for (ELEMENT element = ELE_FIRE; element < ELE_MAX; element=ELEMENT((int)element+1))
    {
        TowerPlacement::CreateTowerAt(element,Point{currentX,startY});
        
        // add a exit collider one square above each tower
        EntityID exitCollider = g_mainGame.RegisterEntity();
        TransformComponent::Add(exitCollider, currentX, startY - Grid::GRID_SQUARE_LENGTH, 0, 1.0f);
        ColliderComponent::Add(exitCollider, Grid::GRID_SQUARE_LENGTH*2, 1, 1);
        EnemyExitComponent::Add(exitCollider);

        // move to next horizontal position based on current tower's range
        currentX += 250;
        
    }
}

static void spawnDebugEnemies()
{
    static int everyXframes = 10;

    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH;
    int startY = Grid::GRID_END_POINT.y - Grid::GRID_SQUARE_LENGTH - 200;
    int currentX = startX;
    // create enemy every 250 horizontal until TOWER_MAX

    if(everyXframes<=0)
    {
        for (TOWER_TYPE towerType = TOWER_FIRE; towerType < TOWER_MAX; towerType = TOWER_TYPE((int)towerType + 1))
        {
            ENEMY_TYPE type = (ENEMY_TYPE)(rand()%ENEMY_LAST_VALUE);
            EnemySpawner::SpawnEnemyAt(&g_mainGame, currentX, startY, type, true);
            
            // move to next horizontal position based on current tower's range
            currentX += 250;
        }
        everyXframes = 40;
    }
    else
    {
        everyXframes--;
    }
    
} 

void MainGameScene::OnLoad() 
{

    WaveSystem_Init(LEVEL_1);

    playerLife_init();

    TowerPlacement::Init();

    Grid::LoadLevel(levelPath);

    if (!strcmp(levelPath, "lvl2.csv"))
    {
        loadDebugLevelTowers();
    }
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

    if (Input::IsKeyDown(SDL_SCANCODE_SPACE))
    {
        WaveSystem_StartNextWave();
    }
}

static const char* TowerTypeToString(TOWER_TYPE type)
{
    switch(type)
    {
        case TOWER_FIRE: return "FIRE";
        case TOWER_WATER: return "WATER";
        case TOWER_FIREWATER: return "FIREWATER";
        case TOWER_EARTH: return "EARTH";
        case TOWER_AIR: return "AIR";
        case TOWER_ELECTRIC: return "ELECTRIC";
        case TOWER_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

static void PrintDebugTowerStats()
{
    // track KPS over last 10 seconds using proper time-based tracking
    static float lastUpdateTime = 0.0f;
    static int lastKills[TOWER_MAX] = {0};
    static float kpsValues[TOWER_MAX] = {0.0f};
    static EntityID statTextEntities[TOWER_MAX] = {0};
    static EntityID killsTextEntities[TOWER_MAX] = {0};
    
    // update KPS every 0.5 seconds for smoother display
    float currentTime = SDL_GetTicks() / 1000.0f;
    if (currentTime - lastUpdateTime >= 0.5f)
    {
        for (int i = TOWER_FIRE; i < TOWER_MAX; i++)
        {
            int currentKills = g_Game.debugTowerKills[i];
            int killDifference = currentKills - lastKills[i];
            float timeDiff = currentTime - lastUpdateTime;
            
            // calculate instantaneous KPS and smooth it
            float instantKPS = killDifference / timeDiff;
            kpsValues[i] = kpsValues[i] * 0.7f + instantKPS * 0.3f; // smoothing
            
            lastKills[i] = currentKills;
        }
        lastUpdateTime = currentTime;
    }
    
    // calculate positions below grid, same horizontal spacing as towers
    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH;
    int textY = Grid::GRID_END_POINT.y + 50; // 50 pixels below grid
    int currentX = startX;
    
    for (TOWER_TYPE towerType = TOWER_FIRE; towerType < TOWER_MAX; towerType = TOWER_TYPE((int)towerType + 1))
    {
        // create or update KPS text entity
        if (statTextEntities[towerType] == 0)
        {
            statTextEntities[towerType] = g_mainGame.RegisterEntity();
            TransformComponent::Add(statTextEntities[towerType], currentX, textY, 0, 0.5f);
            TextComponent::Add(statTextEntities[towerType], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // create or update kills text entity (below KPS text)
        if (killsTextEntities[towerType] == 0)
        {
            killsTextEntities[towerType] = g_mainGame.RegisterEntity();
            TransformComponent::Add(killsTextEntities[towerType], currentX, textY + 20, 0, 0.5f);
            TextComponent::Add(killsTextEntities[towerType], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // update KPS text content
        char statText[100];
        snprintf(statText, sizeof(statText), "%s kps: %.1f", 
                TowerTypeToString(towerType), kpsValues[towerType]);
        
        // update KPS text component
        TextComponent* textComp = Get<TextComponent>(statTextEntities[towerType]);
        if (textComp)
        {
            strncpy(textComp->text, statText, sizeof(textComp->text) - 1);
            textComp->text[sizeof(textComp->text) - 1] = '\0';
            textComp->isDirty = true;
        }
        
        // update kills text content
        char killsText[100];
        snprintf(killsText, sizeof(killsText), "kills: %d", g_Game.debugTowerKills[towerType]);
        
        // update kills text component
        TextComponent* killsTextComp = Get<TextComponent>(killsTextEntities[towerType]);
        if (killsTextComp)
        {
            strncpy(killsTextComp->text, killsText, sizeof(killsTextComp->text) - 1);
            killsTextComp->text[sizeof(killsTextComp->text) - 1] = '\0';
            killsTextComp->isDirty = true;
        }
        
        // move to next horizontal position
        currentX += 250;
    }
}

void MainGameScene::OptionalUpdate(float deltaTime)
{
    handle_pause_input();

    WaveSystem_Update(deltaTime, &g_mainGame);

    // update tower placement
    TowerPlacement::Update();

    Grid::DrawGrid();

    TargetingSystem(&g_mainGame);

    ProjectileSpawningSystem(&g_mainGame);

    new_CrowdcontrolSystem(&g_mainGame);

    DamageOnCollisionSystem(&g_mainGame);

    AttackCDSystem(&g_mainGame, deltaTime);

    ExplodeOnXYSystem(&g_mainGame);
    ChainLightningSystem(&g_mainGame);

    ResolveElementSystem(&g_mainGame);


    if (!strcmp(levelPath, "lvl2.csv"))
    {
        spawnDebugEnemies();
    
        PrintDebugTowerStats();
    }
}
