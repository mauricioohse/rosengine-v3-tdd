#pragma once

#include "../../core/ecs/ecs_types.h"
#include "core/ecs/components.h"

struct TowerTextBlock {
    char name[42];
    char projectileType[42];
    char damage[42];
    char attackSpeed[42];
    char specialEffect[42];
};

// tower data lookup table
struct TowerData {
    PROJECTILE_TYPE projectile_type;
    int range;
    float cooldown;
    int damage;
    TextureID tex = TEXTURE_BOX_MISSING;
    TowerTextBlock textBlock;
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