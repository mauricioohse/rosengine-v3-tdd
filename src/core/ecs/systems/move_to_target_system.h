#pragma once
#include "../systems.h"

struct move_to_target_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
}; 