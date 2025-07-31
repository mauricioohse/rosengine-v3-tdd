#pragma once
#include "../core/ecs/ecs_types.h"
#include "../core/input.h"
#include "grid.h"
#include "components.h"

struct TowerPlacement {
    static void Init();
    static void Update();
    static void Destroy();

    static bool TryPlaceTower(ELEMENT type, int mouseX, int mouseY);
    static EntityID CreateTowerAt(ELEMENT type, Point gridPoint);

    static bool isPlacementMode;
    static ELEMENT selectedElement; // 0 = fire, 1 = water, etc.

    // archetypes
    static void CreateFireTower(EntityID tower);

}; 