#pragma once
#include "ecs/components.h"

// Collision utility functions
inline bool CheckCollisionCentered(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float& penetrationX, float& penetrationY) 
{
    // Calculate boundaries for first box (centered)
    float leftA = transformA->x - colliderA->width/2;
    float rightA = transformA->x + colliderA->width/2;
    float topA = transformA->y - colliderA->height/2;
    float bottomA = transformA->y + colliderA->height/2;
    
    // Calculate boundaries for second box (centered)
    float leftB = transformB->x - colliderB->width/2;
    float rightB = transformB->x + colliderB->width/2;
    float topB = transformB->y - colliderB->height/2;
    float bottomB = transformB->y + colliderB->height/2;
    
    // Check if boxes overlap
    if (leftA < rightB && rightA > leftB &&
        topA < bottomB && bottomA > topB) {
        
        // Calculate penetration depths
        penetrationX = (rightA > rightB) ? 
            rightB - leftA : 
            rightA - leftB;
            
        penetrationY = (bottomA > bottomB) ? 
            bottomB - topA : 
            bottomA - topB;
            
        return true;
    }
    
    return false;
} 