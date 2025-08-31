#pragma once

#include "../../core/ecs/ecs_types.h"
#include "core/ecs/components.h"

// tower data lookup table
struct TowerData {

    // data all towers must have:
    PROJECTILE_TYPE projectile_type;
    int range;
    float cooldown;
    int damage;

    // Data that are optional per tower (should all have default values)
    TextureID tex = TEXTURE_BOX_MISSING;
    SDL_Color colorMod = {255, 255, 255, 255};
    int AOE_radius = 1; //in px
    int chain_count = 1;
    int penetration = 1;


};

struct ElementCombo {
    ELEMENT elements[MAX_ELEMENTS]; // sorted elements
    TowerData tower_data;
};

// element combination lookup
struct ElementCombo;

void ResolveElementSystem(SceneBase* scene);
TowerData* GetTowerDataForElements(ELEMENT* elements);
void SortDescendingElementsInPlace(ELEMENT* elements);