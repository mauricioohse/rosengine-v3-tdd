#pragma once
#include "../systems.h"
#include "../../window.h"
#include <map>
#include <string>

struct RenderSystem : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
    
    // Camera properties (we can expand this later)
    float cameraX = 0.0f;
    float cameraY = 0.0f;

private:
    void RenderEntity(TransformComponent* transform, SpriteComponent* sprite);
    void RenderAnimatedEntity(TransformComponent *transform, AnimationComponent *anim);
    static void RenderSpriteEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera);
    static void RenderTextEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera);
    static void RenderUIEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera);
    void RenderTimedSpriteEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera,  float deltaTime);
};

struct RenderColor {
    Uint8 r, g, b, a;
};

extern std::map<std::string, RenderColor> paletteOpaque = {
    {"red", {255, 0, 0, 255}},
    {"green", {0, 255, 0, 255}},
    {"blue", {0, 0, 255, 255}},
    {"white", {255, 255, 255, 255}},
    {"black", {0, 0, 0, 255}},
    {"grey", {122, 122, 122, 255}}
};