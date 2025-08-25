#include "wasd_controller_system.h"
#include <stdio.h>
#include <math.h>
#include "../../engine.h"
#include "../components.h"

void WASDControllerSystem::Init() {
    printf("WASDControllerSystem initialized\n");
    speedMultiplier = 1.0f;
}

void WASDControllerSystem::Update(float deltaTime, std::vector<EntityID> entities) {
    // Loop through all entities
    for ( EntityID entity : entities) {
        // Check if entity has both transform and WASD controller components
        if (HasComponent<TransformComponent, WASDControllerComponent>(entity)) {
            TransformComponent* transform = Get<TransformComponent>(entity);
            WASDControllerComponent* controller = Get<WASDControllerComponent>(entity);

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