#pragma once

#include "../../core/ecs/ecs_types.h"
#include "core/ecs/components.h"

// tower data lookup table
struct TowerData {
    PROJECTILE_TYPE projectile_type;
    int range;
    float cooldown;
    int damage;
    TextureID tex = TEXTURE_BOX_MISSING;
    SDL_Color colorMod = {255, 255, 255, 255};

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