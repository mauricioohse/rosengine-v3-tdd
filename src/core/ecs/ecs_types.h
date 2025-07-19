#pragma once
#include <SDL.h>
#include "macro_utils.h"


// Common type definitions
typedef uint32_t EntityID;
typedef uint32_t ComponentType;

// Constants
#define MAX_ENTITIES 1000
#define INVALID_ENTITY 0

#define xcomponent(component_type, component, id, ...) COMPONENT_##component_type = 1 << id,
// Component type identifiers
enum ComponentTypes {
    COMPONENT_NONE = 0,
#include "components/components.def"
    COMPONENT_WASD_CONTROLLER = 1 << 2,
    COMPONENT_COLLIDER = 1 << 3,
    COMPONENT_ANIMATION = 1 << 4,
    COMPONENT_GRAVITY = 1 << 5,
    COMPONENT_SQUIRREL = 1 << 6,
    COMPONENT_CAMERA = 1 << 7,
    COMPONENT_CLOUD = 1 << 8,
    COMPONENT_BACKGROUND = 1 << 9,
    COMPONENT_PEANUT = 1 << 10,
    // Add more component types here
}; 
#undef xcomponent

#define ADD_WASD_CONTROLLER(entity, speed, enabled) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_WASD_CONTROLLER); \
        InitWASDController(entity, speed, enabled); \
    } while(0)

#define ADD_COLLIDER(entity, width, height, isStatic, isTrigger) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_COLLIDER); \
        InitCollider(entity, width, height, isStatic, isTrigger); \
    } while(0)

#define ADD_ANIMATION(entity, sheet, frameW, frameH, cols, frames, time, shouldLoop) \
    do { \
        AnimationComponent* anim = (AnimationComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_ANIMATION); \
        if (anim) { \
            anim->Init(sheet, frameW, frameH, cols, frames, time, shouldLoop); \
        } \
    } while(0)

#define ADD_CAMERA(entity, viewportWidth, viewportHeight, targetEntity) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_CAMERA); \
        InitCamera(entity, viewportWidth, viewportHeight, targetEntity); \
    } while(0)

#define ADD_BACKGROUND(entity, parallax) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, COMPONENT_BACKGROUND); \
        BackgroundComponent* background = (BackgroundComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_BACKGROUND); \
        if (background) { \
            background->Init(parallax); \
        } \
    } while(0)

