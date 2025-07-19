#include "main_game_scene.h"
#include "../core/engine.h"
#include "../core/window.h"
#include "game.h"
#include "pause_scene.h"
#include "../core/input.h"
#include "grid.h"
#include "tower_placement.h"


MainGameScene g_mainGame;

static void loadDebugLevelTowers()
{
    // loads all towers types spaced horizontally
    // spawn one tower of each type, each in its own row, spaced by radius
    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH;
    int startY = Grid::GRID_START_POINT.y + Grid::GRID_SQUARE_LENGTH;
    int currentX = startX;

    for (TOWER_TYPE towerType = TOWER_FIRE; towerType < TOWER_MAX; towerType=TOWER_TYPE((int)towerType+1))
    {
        TowerPlacement::selectedTowerType = towerType;
        TowerPlacement::CreateTowerAt(Point{currentX,startY});
        
        // add a exit collider one square above each tower
        EntityID exitCollider = g_mainGame.RegisterEntity();
        ADD_TRANSFORM(exitCollider, currentX, startY - Grid::GRID_SQUARE_LENGTH, 0, 1.0f);
        ADD_COLLIDER(exitCollider, Grid::GRID_SQUARE_LENGTH, Grid::GRID_SQUARE_LENGTH, 1, 1);
        g_Engine.entityManager.AddComponentToEntity(exitCollider, COMPONENT_ENEMYEXIT);

        // move to next horizontal position based on current tower's range
        currentX += 250;
        
    }
}

static void spawnDebugEnemies()
{
    static int everyXframes = 10;

    int startX = Grid::GRID_START_POINT.x + Grid::GRID_SQUARE_LENGTH;
    int startY = Grid::GRID_END_POINT.y - Grid::GRID_SQUARE_LENGTH;
    int currentX = startX;
    // create enemy every 250 horizontal until TOWER_MAX

    if(everyXframes<=0)
    {
        for (TOWER_TYPE towerType = TOWER_FIRE; towerType < TOWER_MAX; towerType = TOWER_TYPE((int)towerType + 1))
        {
            EntityID enemy = g_mainGame.RegisterEntity();
            Texture *tex = ResourceManager::GetTexture(TEXTURE_BOX_ENEMY);
            ADD_SPRITE(enemy, tex);
            ADD_TRANSFORM(enemy, currentX, startY, 0, 0.1f);
            ADD_COLLIDER(enemy, 1, 1, 0, 0);
            ADD_ENEMY(enemy, 100);
            ADD_DEBUGPATH(enemy, towerType);

            // move to next horizontal position based on current tower's range
            currentX += 250;
        }
        everyXframes = 10;
    }
    else
    {
        everyXframes--;
    }
    
} 

void MainGameScene::OnLoad() 
{
    EntityManager* EM = &g_Engine.entityManager;
    
    EntityID box = RegisterEntity();
    EM->AddComponentToEntity(box, COMPONENT_TRANSFORM | COMPONENT_SPRITE | COMPONENT_WASD_CONTROLLER);
    g_Engine.componentArrays.Transforms[box].Init(300, 300);
    g_Engine.componentArrays.Sprites[box].Init(ResourceManager::GetTexture(TEXTURE_BOX));
    g_Engine.componentArrays.wasdControllers[box].Init(600);

    // initialize tower placement system
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
            ADD_TRANSFORM(statTextEntities[towerType], currentX, textY, 0, 0.5f);
            ADD_TEXT(statTextEntities[towerType], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // create or update kills text entity (below KPS text)
        if (killsTextEntities[towerType] == 0)
        {
            killsTextEntities[towerType] = g_mainGame.RegisterEntity();
            ADD_TRANSFORM(killsTextEntities[towerType], currentX, textY + 20, 0, 0.5f);
            ADD_TEXT(killsTextEntities[towerType], ResourceManager::GetFont(FONT_FPS), "");
        }
        
        // update KPS text content
        char statText[100];
        snprintf(statText, sizeof(statText), "%s kps: %.1f", 
                TowerTypeToString(towerType), kpsValues[towerType]);
        
        // update KPS text component
        TextComponent* textComp = (TextComponent*)g_Engine.componentArrays.GetComponentData(
            statTextEntities[towerType], COMPONENT_TEXT);
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
        TextComponent* killsTextComp = (TextComponent*)g_Engine.componentArrays.GetComponentData(
            killsTextEntities[towerType], COMPONENT_TEXT);
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

    // update tower placement
    TowerPlacement::Update();

    Grid::DrawGrid();

    if (!strcmp(levelPath, "lvl2.csv"))
    {
        spawnDebugEnemies();
    
        PrintDebugTowerStats();
    }
}
