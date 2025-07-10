#include "tower_placement.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include "../core/ecs/components.h"
#include "main_game_scene.h"
#include "grid.h"

bool TowerPlacement::isPlacementMode = false;
int TowerPlacement::selectedTowerType = 0;

void TowerPlacement::Init() {
    isPlacementMode = true;
    selectedTowerType = 0;
}

void TowerPlacement::Update() {
    // handle input for tower placement
    static bool mousePressed = false;
    
    if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT)) {
        if (!mousePressed) {
            mousePressed = true;
            
            int mouseX, mouseY;
            Input::GetMousePosition(mouseX, mouseY);
            
            if (TryPlaceTower(mouseX, mouseY)) {
                printf("tower placed successfully\n");
            }
        }
    } else {
        mousePressed = false;
    }
    
    // handle keyboard input for tower selection
    if (Input::IsKeyPressed(SDL_SCANCODE_1)) {
        selectedTowerType = 0; // fire tower
        isPlacementMode = true;
        printf("fire tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_2)) {
        selectedTowerType = 1; // water tower
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_3)) {
        selectedTowerType = 2; // water+fire tower
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_4)) {
        selectedTowerType = 3; // enemy, debug
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        isPlacementMode = false;
        printf("placement mode cancelled\n");
    }
}

void TowerPlacement::Destroy() {
    isPlacementMode = false;
    selectedTowerType = 0;
}

bool TowerPlacement::TryPlaceTower(int mouseX, int mouseY) {
    if (!isPlacementMode) return false;
    
    if (!Grid::IsInsideGrid(mouseX, mouseY)) return false;
    
    Point gridPoint = Grid::GetNearestGridPoint(mouseX, mouseY);
    EntityID tower = CreateTowerAt(gridPoint);
    
    if (tower != INVALID_ENTITY) {
        isPlacementMode = false; // exit placement mode after successful placement
        return true;
    }
    
    return false;
}

EntityID TowerPlacement::CreateTowerAt(Point gridPoint) {
    // create tower entity
    EntityID tower = g_mainGame.RegisterEntity();
    
    printf("attempting to create tower entityId %d\n" , tower);
    if (tower == INVALID_ENTITY) {
        printf("failed to create tower entity\n");
        return INVALID_ENTITY;
    }
    
    // add basic components
    ADD_COLLIDER(tower, 48, 48, 1, 0);
    ADD_TRANSFORM(tower,
        (float)gridPoint.x + Grid::GRID_SQUARE_LENGTH/2, 
        (float)gridPoint.y + Grid::GRID_SQUARE_LENGTH/2,
        0.0F,
        0.5F );

    // initialize sprite (you'll need to add tower textures to resource manager)
    // for now using the box texture as placeholder

    Texture * tex;
    switch (selectedTowerType)
    {
    default:
    case 0: // fire
        ADD_TOWER(tower, 1, 150, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX);
        break;

    case 1: // water
        ADD_TOWER(tower, 2, 100, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX_BLUE);
    break;
    
    case 2: // water + fire
        ADD_TOWER(tower, 3, 200, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX_MIX);
        break;

    case 3: // DEBUG: PLACES ENEMIES
        tex = ResourceManager::GetTexture(TEXTURE_BOX_ENEMY); 
        g_Engine.componentArrays.Transforms[tower].scale=0.1f;
        ADD_ENEMY(tower);

    }
    ADD_SPRITE(tower, tex);
    
    // initialize collider (static, not trigger)
    g_Engine.componentArrays.colliders[tower].Init(48, 48, true, false);

    return tower;
} 