#include "systems.h"


struct tower_behavior_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

    void DrawRangeAroundTower(float x, float y, int radius);

}; 