#include "collision_system.h"
#include "../../utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../engine.h"
#include <algorithm>

void CollisionSystem::Init() {
    printf("CollisionSystem initialized\n");
    collisionCount = 0;
}

bool CollisionSystem::CheckCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float& penetrationX, float& penetrationY) 
{
    return CheckCollisionCentered(transformA, colliderA, transformB, colliderB, 
                                penetrationX, penetrationY);
}

void CollisionSystem::ResolveCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float penetrationX, float penetrationY) 
{
    if (colliderA->isTrigger || colliderB->isTrigger) {
        return;
    }

    // Calculate centers (already at center, so just use transform positions)
    float centerAx = transformA->x;
    float centerAy = transformA->y;
    float centerBx = transformB->x;
    float centerBy = transformB->y;

    // Determine direction of collision
    float directionX = centerAx - centerBx;  // Positive if A is to the right of B
    float directionY = centerAy - centerBy;  // Positive if A is below B

    // Determine which axis has the smaller penetration
    if (abs(penetrationX) < abs(penetrationY)) {
        // Resolve on X axis
        float moveX = (directionX > 0) ? penetrationX : -penetrationX;
        
        if (!colliderA->isStatic && !colliderB->isStatic) {
            transformA->x += moveX * 0.5f;
            transformB->x -= moveX * 0.5f;
        } else if (!colliderA->isStatic) {
            transformA->x += moveX;
        } else if (!colliderB->isStatic) {
            transformB->x -= moveX;
        }
    } else {
        // Resolve on Y axis
        float moveY = (directionY > 0) ? penetrationY : -penetrationY;
        
        if (!colliderA->isStatic && !colliderB->isStatic) {
            transformA->y += moveY * 0.5f;
            transformB->y -= moveY * 0.5f;
        } else if (!colliderA->isStatic) {
            transformA->y += moveY;
        } else if (!colliderB->isStatic) {
            transformB->y -= moveY;
        }
    }
}

void CollisionSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    collisionCount = 0;
    
    // Check collisions between all entities with colliders
    for (EntityID entityA : entities) {
        if (!g_Engine.entityManager.HasComponent(entityA, COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
            continue;
        }
        
        TransformComponent* transformA = 
            (TransformComponent*)components->GetComponentData(entityA, COMPONENT_TRANSFORM);
        ColliderComponent* colliderA = 
            (ColliderComponent*)components->GetComponentData(entityA, COMPONENT_COLLIDER);
            
        auto it = std::find(entities.begin(), entities.end(), entityA);
        for (auto it2 = std::next(it); it2 != entities.end(); ++it2) {
            EntityID entityB = *it2;
            if (!g_Engine.entityManager.HasComponent(entityB, COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
                continue;
            }
            
            TransformComponent* transformB = 
                (TransformComponent*)components->GetComponentData(entityB, COMPONENT_TRANSFORM);
            ColliderComponent* colliderB = 
                (ColliderComponent*)components->GetComponentData(entityB, COMPONENT_COLLIDER);
                
            float penetrationX, penetrationY;
            if (CheckCollision(transformA, colliderA, transformB, colliderB, 
                             penetrationX, penetrationY)) 
            {
                // Store collision
                if (collisionCount < MAX_COLLISIONS) {
                    collisions[collisionCount].entityA = entityA;
                    collisions[collisionCount].entityB = entityB;
                    collisions[collisionCount].penetrationX = penetrationX;
                    collisions[collisionCount].penetrationY = penetrationY;
                    collisionCount++;
                }
                
                // Resolve collision
                ResolveCollision(transformA, colliderA, transformB, colliderB,
                               penetrationX, penetrationY);
            }
        }
    }
}

void CollisionSystem::Destroy() {
    printf("CollisionSystem destroyed\n");
} 