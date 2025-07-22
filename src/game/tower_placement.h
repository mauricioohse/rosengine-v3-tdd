#pragma once
#include "../core/ecs/ecs_types.h"
#include "../core/input.h"
#include "grid.h"
#include "components.h"

struct TowerPlacement {
    static void Init();
    static void Update();
    static void Destroy();

    static bool TryPlaceTower(TOWER_TYPE type, int mouseX, int mouseY);
    static EntityID CreateTowerAt(TOWER_TYPE type, Point gridPoint);

    static bool isPlacementMode;
    static TOWER_TYPE selectedTowerType; // 0 = fire, 1 = water, etc.

    // archetypes
    static void CreateFireTower(EntityID tower);

}; 