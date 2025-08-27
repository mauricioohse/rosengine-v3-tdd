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
#include "../../core/ecs/components.h"
#include <cstdlib>


MainGameScene g_mainGame;

static void loadDebugLevelTowers()
{
    // loads all towers types spaced horizontally
    // spawn one tower of each type, each in its own row, spaced by radius
    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH/2;
    int startY = Grid::GRID_START_POINT.y + Grid::GRID_SQUARE_LENGTH/2;
    int currentX = startX;

    for (ELEMENT element = ELE_FIRE; element < ELE_MAX; element=ELEMENT((int)element+1))
    {
        TowerPlacement::CreateTowerAt(element,Point{currentX,startY});
        printf("element %d\n", element);
        
        // add a exit collider one square above each tower
        EntityID exitCollider = g_mainGame.RegisterEntity();
        TransformComponent::Add(exitCollider, currentX, startY - Grid::GRID_SQUARE_LENGTH, 0, 1.0f);
        ColliderComponent::Add(exitCollider, Grid::GRID_SQUARE_LENGTH, 1, 1);
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
        for (ELEMENT element = ELE_FIRE; element < ELE_MAX; element = ELEMENT((int)element + 1))
        {
            ENEMY_TYPE type = ENEMY_BASIC_I;//(ENEMY_TYPE)(rand()%ENEMY_LAST_VALUE);
            EntityID e = EnemySpawner::SpawnEnemyAt(&g_mainGame, currentX, startY, type, true);
            EnemyDebugComponent::Add(e, element);
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

static EntityID speedTextEntity;
static EntityID mousePosTextEntity;
static EntityID victoryTextEntity;

static void Init_speed_text(){
    speedTextEntity = g_mainGame.RegisterEntity();
    TransformComponent::Add(speedTextEntity, 1400, 833, 0, 1.0f);
    char * text = "Press F to change speed: 1x";
    TextComponent::Add(speedTextEntity, ResourceManager::GetFont(FONT_FPS), text);

    // debug: also show mouse position
    mousePosTextEntity = g_mainGame.RegisterEntity();
    TransformComponent::Add(mousePosTextEntity, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 30, 0, 1.0f);
    char * mousePosText = "(0, 0)";
    TextComponent::Add(mousePosTextEntity, ResourceManager::GetFont(FONT_FPS), mousePosText);


    // victory text, will be only shown in screen if all waves ends
    // victory text entity
    EntityID victoryTextEntity = g_mainGame.RegisterEntity();
    TransformComponent::Add(victoryTextEntity, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0, 3.0f);
    auto t = TextComponent::Add(victoryTextEntity, ResourceManager::GetFont(FONT_FPS), "victory!");
    t->visible = false;
}

static void Update_speed_text()
{
    char text[30];
    auto comp = Get<TextComponent>(speedTextEntity);
    snprintf(comp->text, sizeof(comp->text), "Press F to change speed: %dx", g_Engine.speed);
    comp->isDirty = true;

    // update mouse position text
    auto mousePosComp = Get<TextComponent>(mousePosTextEntity);
    snprintf(mousePosComp->text, sizeof(mousePosComp->text), "(%d, %d)", (int)Input::mouseX, (int)Input::mouseY);
    mousePosComp->isDirty = true;

    // checks win condition
    if (WaveSystem_AllWavesComplete())
    {
        // update text based on remaining life: more than 0, yellow vicotry text. 0 or less, show DEFEATED, TRY HARDER
        victoryTextEntity = speedTextEntity + 2; // hacky but works since we know the order
        auto victoryComp = Get<TextComponent>(victoryTextEntity);
        
        if (playerLife_get_health() > 0) {
            // yellow victory text
            snprintf(victoryComp->text, sizeof(victoryComp->text), "VICTORY!");
            victoryComp->color = {255, 255, 0, 255}; // yellow
        } else {
            // red defeat text
            snprintf(victoryComp->text, sizeof(victoryComp->text), "DEFEATED, TRY HARDER");
            victoryComp->color = {255, 0, 0, 255}; // red
        }
        
        victoryComp->visible = true;
        victoryComp->isDirty = true;
    }
}

void MainGameScene::OnLoad() 
{

    WaveSystem_Init(LEVEL_1);

    playerLife_init();

    Init_speed_text();

    TowerPlacement::Init();

    Grid::LoadLevel(levelPath);

    if (!strcmp(levelPath, "lvl2.csv"))
    {
        loadDebugLevelTowers();
    }
}

void MainGameScene::handle_input()
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

    if (Input::IsKeyPressed(SDL_SCANCODE_F))
    {
        g_Engine.ChangeSpeed();
    }

}

static const char* TowerElementToString(ELEMENT type)
{
    switch(type)
    {
        case ELE_FIRE: return "FIRE";
        case ELE_WATER: return "WATER";
        case ELE_EARTH: return "EARTH";
        case ELE_WIND: return "WIND";
        case ELE_ELECTRIC: return "ELECTRIC";
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
    
    for (ELEMENT element = ELE_FIRE; element < ELE_MAX; element=ELEMENT((int)element+1))
    {
        // create or update KPS text entity
        if (statTextEntities[element] == 0)
        {
            statTextEntities[element] = g_mainGame.RegisterEntity();
            TransformComponent::Add(statTextEntities[element], currentX, textY, 0, 0.5f);
            TextComponent::Add(statTextEntities[element], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // create or update kills text entity (below KPS text)
        if (killsTextEntities[element] == 0)
        {
            killsTextEntities[element] = g_mainGame.RegisterEntity();
            TransformComponent::Add(killsTextEntities[element], currentX, textY + 20, 0, 0.5f);
            TextComponent::Add(killsTextEntities[element], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // update KPS text content
        char statText[100];
        snprintf(statText, sizeof(statText), "%s kps: %.1f", 
                TowerElementToString(element), kpsValues[element]);
        
        // update KPS text component
        TextComponent* textComp = Get<TextComponent>(statTextEntities[element]);
        if (textComp)
        {
            strncpy(textComp->text, statText, sizeof(textComp->text) - 1);
            textComp->text[sizeof(textComp->text) - 1] = '\0';
            textComp->isDirty = true;
        }
        
        // update kills text content
        char killsText[100];
        snprintf(killsText, sizeof(killsText), "kills: %d", g_Game.debugTowerKills[element]);
        
        // update kills text component
        TextComponent* killsTextComp = Get<TextComponent>(killsTextEntities[element]);
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
    handle_input();

    Update_speed_text();

    WaveSystem_Update(deltaTime, &g_mainGame);

    // update tower placement
    TowerPlacement::Update();

    Grid::DrawGrid();

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
