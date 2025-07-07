#pragma once
#include "../systems.h"
#include "../entity.h"
#include "../components.h"

struct CameraSystem : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
}; 