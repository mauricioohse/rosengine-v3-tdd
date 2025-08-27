#include "components.h"
#include <stdio.h>
#include "../engine.h"

void InitSprite(EntityID entity, Texture* texture) {
    SpriteComponent* sprite = Get<SpriteComponent>(entity);
    if (sprite) {
        sprite->Init(texture);
    }
}

void InitWASDController(EntityID entity, float moveSpeed, bool canMove) {
    WASDControllerComponent* controller = Get<WASDControllerComponent>(entity);
    if (controller) {
        controller->Init(moveSpeed, canMove);
    }
}

void InitCollider(EntityID entity, float radius, bool isStatic, bool isTrigger) {
    ColliderComponent* collider = Get<ColliderComponent>(entity);
    if (collider) {
        collider->Init(radius, isStatic, isTrigger);
    }
} 

void InitCamera(EntityID entity, float viewportWidth, float viewportHeight, EntityID target) {
    if (entity >= MAX_ENTITIES) return;

    CameraComponent* camera = Get<CameraComponent>(entity);
    if(camera) {
        camera->Init(viewportWidth, viewportHeight, target);
        printf("Camera component initialized for entity %d\n", entity);
    }
}
