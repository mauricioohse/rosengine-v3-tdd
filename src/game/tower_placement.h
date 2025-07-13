#pragma once
#include "../core/ecs/ecs_types.h"
#include "../core/input.h"
#include "grid.h"
#include "components.h"

struct TowerPlacement {
    static void Init();
    static void Update();
    static void Destroy();
    
    static bool TryPlaceTower(int mouseX, int mouseY);
    static EntityID CreateTowerAt(Point gridPoint);
  
private:
    static bool isPlacementMode;
    static TOWER_TYPE selectedTowerType; // 0 = fire, 1 = water, etc.


}; 