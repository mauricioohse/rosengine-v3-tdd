#include "components.h"
#include <stdio.h>
#include "../engine.h"

void* ComponentArrays::GetComponentData(EntityID entity, ComponentType type) {
    if (entity >= MAX_ENTITIES) {
        printf("Warning: Entity ID %u out of bounds\n", entity);
        return nullptr;
    }

    switch (type) {
#define xcomponent(enum, type, id, ...) case COMPONENT_##enum: return &type##s[entity];
#include "components/components.def"
#undef xcomponent
        case COMPONENT_WASD_CONTROLLER: return &wasdControllers[entity];
        case COMPONENT_COLLIDER:  return &colliders[entity];
        case COMPONENT_ANIMATION:  return &animations[entity];
        case COMPONENT_CAMERA:  return &cameras[entity];
        case COMPONENT_BACKGROUND: return &backgrounds[entity];


        default:
            printf("Warning: Unknown component type %u\n", type);
            return nullptr;
    }
}

void ComponentArrays::RemoveComponent(EntityID entity, ComponentType type) {
    Component* component = (Component*)GetComponentData(entity, type);
    if (component) {
        component->Destroy();
    }
}


void InitSprite(EntityID entity, Texture* texture) {
    SpriteComponent* sprite = 
        (SpriteComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_SPRITE);
    if (sprite) {
        sprite->Init(texture);
    }
}

void InitWASDController(EntityID entity, float moveSpeed, bool canMove) {
    WASDControllerComponent* controller = 
        (WASDControllerComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_WASD_CONTROLLER);
    if (controller) {
        controller->Init(moveSpeed, canMove);
    }
}

void InitCollider(EntityID entity, float width, float height, bool isStatic, bool isTrigger) {
    ColliderComponent* collider = 
        (ColliderComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_COLLIDER);
    if (collider) {
        collider->Init(width, height, isStatic, isTrigger);
    }
} 

void InitCamera(EntityID entity, float viewportWidth, float viewportHeight, EntityID target) {
    if (entity >= MAX_ENTITIES) return;
    
    CameraComponent* camera = (CameraComponent*) g_Engine.componentArrays.GetComponentData(entity, COMPONENT_CAMERA);
    if(camera) {
        camera->Init(viewportWidth, viewportHeight, target);
        printf("Camera component initialized for entity %d\n", entity);
    }
}

