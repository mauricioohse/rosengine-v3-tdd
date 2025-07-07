#pragma once
#include "../systems.h"

struct BackgroundSystem : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
}; 