#include "tower_placement.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include "../core/ecs/components.h"
#include "main_game_scene.h"
#include "grid.h"

bool TowerPlacement::isPlacementMode = false;
ELEMENT TowerPlacement::selectedElement = ELE_NONE;

void TowerPlacement::Init() {
    isPlacementMode = true;
    selectedElement = ELE_NONE;
}

void TowerPlacement::Update() {
    // handle input for tower placement
    static bool mousePressed = false;
    
    if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT)) {
        if (!mousePressed) {
            mousePressed = true;
            
            int mouseX, mouseY;
            Input::GetMousePosition(mouseX, mouseY);
            
            if (TryPlaceTower(selectedElement, mouseX, mouseY)) {
                printf("tower placed successfully\n");
            }
            else
            {
                printf("tower not placed!\n");
            }
            
        }
    } else {
        mousePressed = false;
    }
    
    // handle keyboard input for tower selection
    if (Input::IsKeyPressed(SDL_SCANCODE_1)) {
        selectedElement = ELE_FIRE; // fire tower
        isPlacementMode = true;
        printf("fire tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_2)) {
        selectedElement = ELE_WATER; // water tower
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_3)) {
        // selectedElement = ELE_; // water+fire tower
        // isPlacementMode = true;
        // printf("fire_water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_4)) {
        selectedElement = ELE_NONE; // enemy, debug
        isPlacementMode = true;
        printf("enemy selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_Q)) {
        selectedElement = ELE_EARTH;
        isPlacementMode = true;
        printf("earth tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_W)) {
        selectedElement = ELE_WIND;
        isPlacementMode = true;
        printf("air tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_E)) {
        selectedElement = ELE_ELECTRIC;
        isPlacementMode = true;
        printf("electric tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        isPlacementMode = false;
        printf("placement mode cancelled\n");
    }
}

void TowerPlacement::Destroy() {
    isPlacementMode = false;
    selectedElement = ELE_NONE;
}

static bool AddElementToExistingTower(EntityID e, ELEMENT element)
{
    // check the existing elements, only add a new element if it is available
    ElementComponent * container = GET_Element(e);
    if(!container) {
        printf("Existing tower has no element! invalid!\n");
        return false;
    }
    int idx=0;
    while(container->elements[idx]!=ELE_NONE)
    {
        idx++;
    }
    
    if (idx >= MAX_ELEMENTS)
    {
        printf("Max element achieved! cant add anymore!\n");
        return false;
    }
    else
    {
        ADD_Element(e, element);
        ADD_ResolveElement(e);
        return true;
    }
}

bool TowerPlacement::TryPlaceTower(ELEMENT type, int mouseX, int mouseY) {
    if (!isPlacementMode) return false;
    
    if (!Grid::IsInsideGrid(mouseX, mouseY)) return false;
    
    Point gridPoint = Grid::GetNearestGridPointCenter(mouseX, mouseY);

    // TODO: check if tower has elements slot available, if not return
    SceneBase * scene = &g_mainGame;
    EntityID existingTower = 0;
    FOR_EACH_COMPONENT_2(scene, tower,
                          Transform, TR,
                          Tower, TC
                          )
    {
        if (TR->x == gridPoint.x && TR->y == gridPoint.y)
        {
            existingTower = tower;
        }
    } END_FOR_EACH

    EntityID new_tower = 0;
    if (existingTower != INVALID_ENTITY)
    {
        if (!AddElementToExistingTower(existingTower, (ELEMENT)selectedElement))
            return false; // TODO: needs to change this to be element instead of tower type lol
    }
    else
    {
        new_tower = CreateTowerAt(type, gridPoint);
    }

    if (existingTower != INVALID_ENTITY || new_tower != INVALID_ENTITY) {
        isPlacementMode = true; // exit placement mode after successful placement
        return true;
    }
    
    return false;
}

EntityID TowerPlacement::CreateTowerAt(ELEMENT type, Point gridPoint) {
    if (selectedElement == TOWER_NONE) {
        printf("no tower type selected\n");
        return INVALID_ENTITY;
    }
    
    // create tower entity
    EntityID tower = g_mainGame.RegisterEntity();
    Texture * tex;

    Point center = Grid::GetNearestGridPointCenter(gridPoint.x, gridPoint.y); 
    
    printf("attempting to create tower entityId %d with element %s\n" , tower, GetElementName(type));
    if (tower == INVALID_ENTITY) {
        printf("failed to create tower entity\n");
        return INVALID_ENTITY;
    }
    
    // add basic components
    ADD_Collider(tower, 48, 48, 1, 0);
    ADD_Transform(tower,
        (float)center.x, 
        (float)center.y,
        0.0F,
        0.5F );


    // note/TODO: this below is just for debugging purposes adding enemies. in the future will be removed.
    if(type == TOWER_NONE)
    {
        tex = ResourceManager::GetTexture(TEXTURE_BOX_ENEMY);
        g_Engine.componentArrays.Transforms[tower].scale = 0.1f;
        ADD_Collider(tower, 1, 1, 0, 0);
        g_Engine.componentArrays.Colliders[tower].height = g_Engine.componentArrays.Sprites[tower].height; // copy the collider size from the sprite size
        g_Engine.componentArrays.Colliders[tower].width = g_Engine.componentArrays.Sprites[tower].width;
        ADD_Enemy(tower, 100);
        ADD_Sprite(tower, tex);
        return tower;
    }


    ADD_Tower(tower, TOWER_NONE, 125, 2); // TODO: remove unused tower data in tower component
    tex = ResourceManager::GetTexture(TEXTURE_BOX_MISSING); // NOTE: the texture will be changed on resolve elements, this is here for easier debugging
    ADD_Element(tower, type);
    ADD_ResolveElement(tower);
    ADD_Sprite(tower, tex);
    return tower;

    // switch (type)
    // {
    // default:
    // case TOWER_FIRE: // fire
    //     ADD_Tower(tower, type, 125, 2);
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_MISSING);
    //     ADD_Element(tower, ELE_FIRE);
    //     ADD_ResolveElement(tower);
    //     // ADD_Target(tower, 0);
    //     // ADD_Damage(tower,50);
    //     // ADD_Cooldown(tower, 2);
    //     // ADD_ProjectileSpawner(tower, PROJECTILE_BOMB);

    //     break;

    // case TOWER_WATER: // water
    //     ADD_Tower(tower, type, 250, 2);
    //     ADD_Target(tower, 0);
    //     ADD_Damage(tower, 200);
    //     ADD_Cooldown(tower,2);
    //     ADD_ProjectileSpawner(tower,PROJECTILE_JET);
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_BLUE);
    //     break;

    // case TOWER_EARTH:
    //     ADD_Tower(tower, type, 150, .33f);
    //     ADD_Target(tower, 0);
    //     ADD_Damage(tower, 150);
    //     ADD_Cooldown(tower, .33f);
    //     ADD_ProjectileSpawner(tower,PROJECTILE_PELLET);
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_EARTH);
    //     break;

    // case TOWER_AIR:
    //     ADD_Tower(tower, type, 150, 1.5);
    //     ADD_Target(tower, 0);
    //     ADD_Cooldown(tower, 2);
    //     ADD_ProjectileSpawner(tower, PROJECTILE_GUST);
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_AIR);
    //     break;

    // case TOWER_ELECTRIC:
    //     ADD_Tower(tower, type, 150, 3);
    //     ADD_Cooldown(tower, 3);
    //     ADD_Target(tower, 0);
    //     ADD_Damage(tower, 150);
    //     ADD_ProjectileSpawner(tower, PROJECTILE_LIGHTNING);
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_ELECTRO);
    //     break;

    // case TOWER_FIREWATER: // water + fire
    //     ADD_Tower(tower, type, 200, 2);
    //     ADD_ProjectileSpawner(tower, PROJECTILE_JET_BOMB);
    //     ADD_Damage(tower, 100);
    //     ADD_Target(tower, 0);
    //     ADD_Cooldown(tower, 2);

    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_FIRE_WATER);
    //     break;

    // case TOWER_DEBUG: // DEBUG: PLACES ENEMIES
    //     tex = ResourceManager::GetTexture(TEXTURE_BOX_ENEMY); 
    //     g_Engine.componentArrays.Transforms[tower].scale=0.1f;
    //     ADD_Collider(tower, 1, 1, 0 ,0);
    //     g_Engine.componentArrays.Colliders[tower].height=g_Engine.componentArrays.Sprites[tower].height; // copy the collider size from the sprite size
    //     g_Engine.componentArrays.Colliders[tower].width=g_Engine.componentArrays.Sprites[tower].width;
    //     ADD_Enemy(tower, 100);
    // }
    

}

