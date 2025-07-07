#include "wasd_controller_system.h"
#include <stdio.h>
#include <math.h>
#include "../../engine.h"

void WASDControllerSystem::Init() {
    printf("WASDControllerSystem initialized\n");
    speedMultiplier = 1.0f;
}

void WASDControllerSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Loop through all entities
    for ( EntityID entity : entities) {
        // Check if entity has both transform and WASD controller components
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_WASD_CONTROLLER )) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            WASDControllerComponent* controller = 
                (WASDControllerComponent*)components->GetComponentData(entity, COMPONENT_WASD_CONTROLLER);
            
            if (!transform || !controller || !controller->canMove) {
                continue;
            }

            // Apply input forces with speed multiplier
            float moveForce = controller->moveSpeed * speedMultiplier;
            
            // Reset movement values
            controller->moveX = 0;
            controller->moveY = 0;
            // Handle WASD movement
            if (Input::IsKeyDown(SDL_SCANCODE_W)) {
                transform->y -= moveForce * deltaTime;
                controller->moveY = -1;
            }
            if (Input::IsKeyDown(SDL_SCANCODE_S)) {
                transform->y += moveForce * deltaTime;
                controller->moveY = 1;
            }
            if (Input::IsKeyDown(SDL_SCANCODE_A)) {
                transform->x -= moveForce * deltaTime;
                controller->moveX = -1;
            }
            if (Input::IsKeyDown(SDL_SCANCODE_D)) {
                transform->x += moveForce * deltaTime;
                controller->moveX = 1;
            }
        }
    }
}

void WASDControllerSystem::Destroy() {
    printf("WASDControllerSystem destroyed\n");
} 