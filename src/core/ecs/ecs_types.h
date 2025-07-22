#pragma once
#include <SDL.h>
#include "macro_utils.h"


// Common type definitions
typedef uint32_t EntityID;
typedef uint32_t ComponentType;

// Constants
#define MAX_ENTITIES 1000
#define INVALID_ENTITY 0

#define xcomponent( component, id, ...) C_##component = 1 << id,
// Component type identifiers
enum ComponentTypes {
    C_NONE = 0,
#include "components/components.def"
    C_WASD_CONTROLLER = 1 << 2,
    C_COLLIDER = 1 << 3,
    C_ANIMATION = 1 << 4,
    C_GRAVITY = 1 << 5,
    C_SQUIRREL = 1 << 6,
    C_CAMERA = 1 << 7,
    C_CLOUD = 1 << 8,
    C_BACKGROUND = 1 << 9,
    C_PEANUT = 1 << 10,
    // Add more component types here
}; 
#undef xcomponent

#define ADD_WASD_CONTROLLER(entity, speed, enabled) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, C_WASD_CONTROLLER); \
        InitWASDController(entity, speed, enabled); \
    } while(0)

#define ADD_COLLIDER(entity, width, height, isStatic, isTrigger) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, C_COLLIDER); \
        InitCollider(entity, width, height, isStatic, isTrigger); \
    } while(0)

#define ADD_ANIMATION(entity, sheet, frameW, frameH, cols, frames, time, shouldLoop) \
    do { \
        AnimationComponent* anim = (AnimationComponent*)g_Engine.componentArrays.GetComponentData(entity, C_ANIMATION); \
        if (anim) { \
            anim->Init(sheet, frameW, frameH, cols, frames, time, shouldLoop); \
        } \
    } while(0)

#define ADD_CAMERA(entity, viewportWidth, viewportHeight, targetEntity) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, C_CAMERA); \
        InitCamera(entity, viewportWidth, viewportHeight, targetEntity); \
    } while(0)

#define ADD_BACKGROUND(entity, parallax) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, C_BACKGROUND); \
        BackgroundComponent* background = (BackgroundComponent*)g_Engine.componentArrays.GetComponentData(entity, C_BACKGROUND); \
        if (background) { \
            background->Init(parallax); \
        } \
    } while(0)

