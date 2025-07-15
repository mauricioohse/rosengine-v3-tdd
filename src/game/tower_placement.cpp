#include "tower_placement.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include "../core/ecs/components.h"
#include "main_game_scene.h"
#include "grid.h"

bool TowerPlacement::isPlacementMode = false;
TOWER_TYPE TowerPlacement::selectedTowerType = TOWER_NONE;

void TowerPlacement::Init() {
    isPlacementMode = true;
    selectedTowerType = TOWER_NONE;
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
        selectedTowerType = TOWER_FIRE; // fire tower
        isPlacementMode = true;
        printf("fire tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_2)) {
        selectedTowerType = TOWER_WATER; // water tower
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_3)) {
        selectedTowerType = TOWER_FIREWATER; // water+fire tower
        isPlacementMode = true;
        printf("fire_water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_4)) {
        selectedTowerType = TOWER_DEBUG; // enemy, debug
        isPlacementMode = true;
        printf("enemy selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_Q)) {
        selectedTowerType = TOWER_EARTH;
        isPlacementMode = true;
        printf("earth tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        isPlacementMode = false;
        printf("placement mode cancelled\n");
    }
}

void TowerPlacement::Destroy() {
    isPlacementMode = false;
    selectedTowerType = TOWER_NONE;
}

bool TowerPlacement::TryPlaceTower(int mouseX, int mouseY) {
    if (!isPlacementMode) return false;
    
    if (!Grid::IsInsideGrid(mouseX, mouseY)) return false;
    
    Point gridPoint = Grid::GetNearestGridPoint(mouseX, mouseY);
    EntityID tower = CreateTowerAt(gridPoint);
    
    if (tower != INVALID_ENTITY) {
        isPlacementMode = true; // exit placement mode after successful placement
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
    case TOWER_FIRE: // fire
        ADD_TOWER(tower, selectedTowerType, 125, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX);
        break;

    case TOWER_WATER: // water
        ADD_TOWER(tower, selectedTowerType, 250, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX_BLUE);
        break;

    case TOWER_EARTH:
        ADD_TOWER(tower, selectedTowerType, 150, .33f);
        tex = ResourceManager::GetTexture(TEXTURE_BOX_EARTH);
        break;

    case TOWER_FIREWATER: // water + fire
        ADD_TOWER(tower, selectedTowerType, 200, 2);
        tex = ResourceManager::GetTexture(TEXTURE_BOX_MIX);
        break;

    case TOWER_DEBUG: // DEBUG: PLACES ENEMIES
        tex = ResourceManager::GetTexture(TEXTURE_BOX_ENEMY); 
        g_Engine.componentArrays.Transforms[tower].scale=0.1f;
        ADD_COLLIDER(tower, 1, 1, 0 ,0);
        g_Engine.componentArrays.colliders[tower].height=g_Engine.componentArrays.Sprites[tower].height; // copy the collider size from the sprite size
        g_Engine.componentArrays.colliders[tower].width=g_Engine.componentArrays.Sprites[tower].width;
        ADD_ENEMY(tower, 100);
    }
    ADD_SPRITE(tower, tex);
    
    // initialize collider (static, not trigger)
    g_Engine.componentArrays.colliders[tower].Init(48, 48, true, false);

    return tower;
} 