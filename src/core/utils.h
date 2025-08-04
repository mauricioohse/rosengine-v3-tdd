#pragma once
#include "ecs/components.h"
#include <cmath>

#define RED_BEGIN "\x1B[31m"
#define RED_END  "\x1B[0m"

// Collision utility functions
inline bool CheckCollision(
    TransformComponent* transformA, ColliderComponent* colliderA,
    TransformComponent* transformB, ColliderComponent* colliderB,
    float& penetrationX, float& penetrationY) 
{
    // Calculate distance between circles centered on two entities
    float distancep2 = pow(abs(transformB->x - transformA->x), 2) + pow(abs(transformB->y - transformA->y), 2);  // avoiding use of sqrt
    
    // Check if circles overlap
    if (distancep2 <= pow((colliderA->radius + colliderB->radius), 2)) {
        
        // Calculate penetration depths
        //penetrationX = (rightA > rightB) ? 
        //    rightB - leftA : 
        //    rightA - leftB;
        //    
        //penetrationY = (bottomA > bottomB) ? 
        //    bottomB - topA : 
        //    bottomA - topB;
        //    
        return true;
    }
    
    return false;
} 