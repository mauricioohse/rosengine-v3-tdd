#pragma once
#include "../systems.h"

struct life_time_system : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
}; 