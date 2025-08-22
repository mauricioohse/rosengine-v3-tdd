#include "tower_placement.h"
#include "../core/engine.h"
#include "../core/resource_manager.h"
#include "../core/ecs/components.h"
#include "main_game_scene.h"
#include "grid.h"
#include "enemy_spawner.h"
#include "wave_system.h"
#include "play_sound.h"


bool TowerPlacement::isPlacementMode = false;
ELEMENT TowerPlacement::selectedElement = ELE_NONE;
static EntityID texts[10];
static TextComponent textscomp[10];


static void InitSelectionText()
{
    // Init the selection text entities, but keep them invisible until needed


    for(int i = 0; i < 4; i++) {
        texts[i] = g_mainGame.RegisterEntity();
        
        auto textComp = ADD_Text(texts[i], ResourceManager::GetFont(FONT_FPS), "");
        ADD_Transform(texts[i], 1580, 300+(i*40),0,1.3);
        textComp->visible = false; // start invisible
        textComp->alignment = TEXT_RIGHT;
        switch(i) {
            case 0:
            strncpy(textComp->text, "Select new element:", sizeof(textComp->text) - 1);
                break;
            case 1:
                strncpy(textComp->text, "1: fire tower", sizeof(textComp->text) - 1);
                break;
            case 2:
                strncpy(textComp->text, "2: water tower", sizeof(textComp->text) - 1);
                break;
            case 3:
                strncpy(textComp->text, "3: air tower", sizeof(textComp->text) - 1);
                break;
        }
    }
}

void TowerPlacement::Init() {
    isPlacementMode = true;
    selectedElement = ELE_NONE;
    InitSelectionText();

}

void TowerPlacement::HandleSelection()
{
    const SDL_Color yellowColor = {255, 255, 0, 255};

    // allow the inputs 1 to 3 to select fire, water or air tower
    if (Input::IsKeyPressed(SDL_SCANCODE_1))
    {
        selectedElement = ELE_FIRE; // fire tower
        isPlacementMode = true;
        printf("fire tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_2))
    {
        selectedElement = ELE_WATER; // water tower
        isPlacementMode = true;
        printf("water tower selected\n");
    }
    if (Input::IsKeyPressed(SDL_SCANCODE_3))
    {
        selectedElement = ELE_WIND; // water+fire tower
        isPlacementMode = true;
        printf("air tower selected\n");
    }

    // now update the colors: the selected text should be yellow, others should be black
    for(int i = 0; i < 4; i++) {
        auto textComp = GET_Text(texts[i]);
        if(textComp) {
            if((i == 1 && selectedElement == ELE_FIRE) ||
               (i == 2 && selectedElement == ELE_WATER) ||
               (i == 3 && selectedElement == ELE_WIND)) {
                textComp->color = {255, 255, 0, 255}; // yellow for selected
                textComp->isDirty = true;
            } else {
                if (memcmp(&textComp->color, &yellowColor, sizeof(textComp->color)) == 0)
                {
                    textComp->isDirty = true;
                    textComp->color = {0, 0, 0, 255}; // black for unselected
                }
            }
            // also makes it visible
            textComp->visible=true;
        }
    }


}


void TowerPlacement::Update() {
    // handle input for tower placement
    static bool mousePressed = false;
    
    if(WaveSystem_IsInTowerSelection())
    {
        HandleSelection();
    }
    else
    {
        // make text invisible
        for(int i = 0; i < 4; i++) {
            auto textComp = GET_Text(texts[i]);
            if(textComp) {
                textComp->visible = false;
            }
        }
    }

    if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT)) {
        if (!mousePressed) {
            mousePressed = true;
            
            int mouseX, mouseY;
            Input::GetMousePosition(mouseX, mouseY);
            
            if (TryPlaceTower(selectedElement, mouseX, mouseY)) {
                printf("tower placed successfully\n");
                PlaySound::PlaySound(SOUND_JUMP);
                
                // if we're in tower selection mode, complete the selection
                if (WaveSystem_IsInTowerSelection()) {
                    WaveSystem_StartNextWave();
                    isPlacementMode = false; // exit placement mode
                }
            }
            else
            {
                printf("tower not placed!\n");
            }
            
        }
    } else {
        mousePressed = false;
    }
    
    // TODO: temporarely disabling debug tower placement. in future, we should have a way to have this enabled (maybe a terminal command or something else)
    if (1) return;
    
    // only allow manual tower selection if NOT in wave system tower selection mode
    if (!WaveSystem_IsInTowerSelection()) {
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
    if (type == TOWER_NONE) {
        // TODO: in the future, deal with debug enemy spawning properly instead of here
        EnemySpawner::SpawnEnemyAt(&g_mainGame, gridPoint.x, gridPoint.y, ENEMY_BASIC_I);
        printf("no tower type selected, creating debug enemy for the moment\n");
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
    ADD_Collider(tower, 48, 1, 0);
    ADD_Transform(tower,
        (float)center.x, 
        (float)center.y,
        0.0F,
        0.5F );



    ADD_Tower(tower, TOWER_NONE, 125, 2); // TODO: remove unused tower data in tower component
    tex = ResourceManager::GetTexture(TEXTURE_BOX_MISSING); // NOTE: the texture will be changed on resolve elements, this is here for easier debugging
    ADD_Element(tower, type);
    ADD_ResolveElement(tower);
    ADD_Sprite(tower, tex);
    return tower;


    

}

