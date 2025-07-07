#pragma once
#include "../systems.h"

class TextUpdateSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
}; 