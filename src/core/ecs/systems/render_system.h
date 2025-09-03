#pragma once
#include "../systems.h"
#include "../../window.h"
#include "../components.h"
#include <map>
#include <string>

struct RenderSystem : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities) override;
    void Destroy() override;
    static void InitTowerText();
    
    // Camera properties (we can expand this later)
    float cameraX = 0.0f;
    float cameraY = 0.0f;

private:
    void RenderEntity(TransformComponent* transform, SpriteComponent* sprite);
    void RenderAnimatedEntity(TransformComponent *transform, AnimationComponent *anim);
    static void RenderSpriteEntity(EntityID entity, CameraComponent* camera);
    static void RenderTextEntity(EntityID entity, CameraComponent* camera);
    static void RenderUIEntity(EntityID entity, CameraComponent* camera);
    void RenderTimedSpriteEntity(EntityID entity, CameraComponent* camera,  float deltaTime);
};

struct RenderColor {
    Uint8 r, g, b, a;
};

extern std::map<std::string, RenderColor> paletteOpaque;