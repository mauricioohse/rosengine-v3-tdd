#pragma once
#include "../systems.h"

class TextUpdateSystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities) override;
    void Destroy() override;
}; 