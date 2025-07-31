#pragma once
#include <SDL.h>
#include "macro_utils.h"


// Common type definitions
typedef uint32_t EntityID;
typedef uint64_t ComponentType;

// Constants
#define MAX_ENTITIES 1000
#define INVALID_ENTITY 0

#define xcomponent( component, id, ...) C_##component = 1ULL << id,
// Component type identifiers
enum ComponentTypes : uint64_t {
    C_NONE = 0,
#include "components/components.def"
    C_WASD_CONTROLLER = 1ULL << 2,
    C_ANIMATION = 1ULL << 4,
    C_GRAVITY = 1ULL << 5,
    C_SQUIRREL = 1ULL << 6,
    C_CAMERA = 1ULL << 7,
    C_CLOUD = 1ULL << 8,
    C_BACKGROUND = 1ULL << 9,
    C_PEANUT = 1ULL << 10,
    // Add more component types here
}; 
#undef xcomponent

#define ADD_WASD_CONTROLLER(entity, speed, enabled) \
    do { \
        g_Engine.entityManager.AddComponentToEntity(entity, C_WASD_CONTROLLER); \
        InitWASDController(entity, speed, enabled); \
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

