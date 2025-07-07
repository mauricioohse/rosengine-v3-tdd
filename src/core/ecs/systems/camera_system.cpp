#include "camera_system.h"
#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "../../engine.h"

void CameraSystem::Init() {
    printf("CameraSystem initialized\n");
}

void CameraSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    for ( EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CAMERA)) {
            CameraComponent* camera = &components->cameras[entity];
            
            if (camera->targetEntity == 0) continue;
            
            // Get target's transform
            TransformComponent* targetTransform = 
                (TransformComponent*)components->GetComponentData(camera->targetEntity, COMPONENT_TRANSFORM);
            
            if (!targetTransform) continue;

            // Handle screen shake
            float shakeOffsetX = 0.0f;
            float shakeOffsetY = 0.0f;
            
            if (camera->shakeTimer > 0) {
                camera->shakeTimer -= deltaTime;
                if (camera->shakeTimer <= 0) {
                    camera->shakeAmount = 0;
                } else {
                    // Random shake offset based on shake amount
                    shakeOffsetX = ((rand() % 200 - 100) / 100.0f) * camera->shakeAmount;
                    shakeOffsetY = ((rand() % 200 - 100) / 100.0f) * camera->shakeAmount;
                }
            }

            // Gradually reduce camera kick
            if (camera->cameraKick != 0) {
                camera->cameraKick *= 0.95f;  // Reduce kick by 5% each frame
                if (fabs(camera->cameraKick) < 0.1f) {
                    camera->cameraKick = 0;
                }
            }

            // Calculate target position (center of screen) with shake
            camera->targetX = targetTransform->x - camera->viewportWidth/2 + shakeOffsetX;
            camera->targetY = targetTransform->y - camera->viewportHeight/2 + camera->cameraKick + shakeOffsetY;

            // Smooth follow
            camera->x += (camera->targetX - camera->x) * CAMERA_FOLLOW_SPEED*4 * deltaTime;
            camera->y += (camera->targetY - camera->y) * CAMERA_FOLLOW_SPEED * deltaTime;

            // Clamp camera to prevent showing areas outside the game world
            //camera->y = std::max(0.0f, std::min((float)GAME_HEIGHT - camera->viewportHeight, camera->y));
        }
    }
}

void CameraSystem::Destroy() {
    printf("CameraSystem destroyed\n");
} 