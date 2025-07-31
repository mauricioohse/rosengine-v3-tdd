#include "components.h"
#include <stdio.h>
#include "../engine.h"

void* ComponentArrays::GetComponentData(EntityID entity, ComponentType type) {
    if (entity >= MAX_ENTITIES) {
        printf("Warning: Entity ID %u out of bounds\n", entity);
        return nullptr;
    }

    if (!HAS_COMPONENT(entity, type)) return nullptr;

    switch (type) {
#define xcomponent(type, id, ...) case C_##type: return &type##s[entity];
#include "components/components.def"
#undef xcomponent
        case C_WASD_CONTROLLER: return &wasdControllers[entity];
        case C_ANIMATION:  return &animations[entity];
        case C_CAMERA:  return &cameras[entity];
        case C_BACKGROUND: return &backgrounds[entity];


        default:
            printf("Warning: Unknown component type %llu\n", type);
            return nullptr;
    }
}

void ComponentArrays::RemoveComponent(EntityID entity, ComponentType type) {
    Component* component = (Component*)GetComponentData(entity, type);
    if (component) {
        // TODO: we actually never use this function. We just remove the bitmask in the entity manager. this can maybe cause issues in the future
        // of deleted entities and a new entity getting it. ideally, before initializing any component, we should always zero out all data inside it.
        // but so far havent caused any issues!
        component->Destroy();
    }
}


void InitSprite(EntityID entity, Texture* texture) {
    SpriteComponent* sprite = 
        (SpriteComponent*)g_Engine.componentArrays.GetComponentData(entity, C_Sprite);
    if (sprite) {
        sprite->Init(texture);
    }
}

void InitWASDController(EntityID entity, float moveSpeed, bool canMove) {
    WASDControllerComponent* controller = 
        (WASDControllerComponent*)g_Engine.componentArrays.GetComponentData(entity, C_WASD_CONTROLLER);
    if (controller) {
        controller->Init(moveSpeed, canMove);
    }
}

void InitCollider(EntityID entity, float width, float height, bool isStatic, bool isTrigger) {
    ColliderComponent* collider = 
        (ColliderComponent*)g_Engine.componentArrays.GetComponentData(entity, C_Collider);
    if (collider) {
        collider->Init(width, height, isStatic, isTrigger);
    }
} 

void InitCamera(EntityID entity, float viewportWidth, float viewportHeight, EntityID target) {
    if (entity >= MAX_ENTITIES) return;
    
    CameraComponent* camera = (CameraComponent*) g_Engine.componentArrays.GetComponentData(entity, C_CAMERA);
    if(camera) {
        camera->Init(viewportWidth, viewportHeight, target);
        printf("Camera component initialized for entity %d\n", entity);
    }
}



