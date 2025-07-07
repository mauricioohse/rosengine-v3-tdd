#pragma once
#include "../ecs_types.h"
#include "../components.h"
#include "../../input.h"
#include "../../engine.h"

class UISystem : public System {
public:
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;

private:
    static bool IsPointInBox(float pointX, float pointY, float boxX, float boxY, float boxWidth, float boxHeight);
    static void RenderUIBox(const UIBoxComponent& box, const TransformComponent& transform);
    static void RenderUIText(const TextComponent& text, const TransformComponent& transform, const UIBoxComponent& box);
}; 