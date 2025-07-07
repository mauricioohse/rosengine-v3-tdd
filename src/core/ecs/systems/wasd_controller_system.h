#pragma once
#include "../systems.h"
#include "../../input.h"

class WASDControllerSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

    // Getter for stats
    float GetSpeedMultiplier() const { return speedMultiplier; }

private:
    float speedMultiplier;
}; 