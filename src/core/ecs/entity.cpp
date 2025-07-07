#include "entity.h"
#include <stdio.h>

// Initialize static members if any are needed later
EntityManager g_EntityManager;

EntityID EntityManager::CreateEntity() {
    // Check if we've reached the entity limit
    if (entityCount >= MAX_ENTITIES) {
        printf("Warning: Reached maximum entity count!\n");
        return INVALID_ENTITY;
    }
    
    // Find first available entity ID
    // Entity IDs start from 1 since 0 is INVALID_ENTITY
    for (EntityID i = 1; i < MAX_ENTITIES; i++) {
        if (!activeEntities[i]) {
            activeEntities[i] = true;
            componentMasks[i] = 0;
            entityCount++;
            return i;
        }
    }
    
    return INVALID_ENTITY;
}

void EntityManager::DestroyEntity(EntityID entity) {
    if (!activeEntities[entity]) {
        printf("Warning: Attempting to destroy inactive entity %u\n", entity);
        return;
    }
    
    activeEntities[entity] = false;
    componentMasks[entity] = 0;
    entityCount--;
}

bool EntityManager::IsEntityValid(EntityID entity) { 

    return (entity > 0 && 
            entity < MAX_ENTITIES && 
            activeEntities[entity]);
}

void EntityManager::AddComponentToEntity(EntityID entity, ComponentType type) {
    if (!IsEntityValid(entity)) {
        printf("Warning: Attempting to add component to invalid entity %u\n", entity);
        return;
    }
    
    // Add component type to entity's mask using bitwise OR
    componentMasks[entity] |= type;
}

void EntityManager::RemoveComponentFromEntity(EntityID entity, ComponentType type) {
    if (!IsEntityValid(entity)) {
        printf("Warning: Attempting to remove component from invalid entity %u\n", entity);
        return;
    }
    
    // Remove component type from entity's mask using bitwise AND with inverted type
    componentMasks[entity] &= ~type;
    
    // If entity has no more components, destroy it
    if (componentMasks[entity] == 0) {
        DestroyEntity(entity);
    }
}

bool EntityManager::HasComponent(EntityID entity, ComponentType componentMask) {
    if (!IsEntityValid(entity)) {
        return false;
    }
    
    // Check if all bits from componentMask are present in entity's mask
    return (componentMasks[entity] & componentMask) == componentMask;
}

void EntityManager::Init() {
    entityCount = 0;
    for (EntityID i = 0; i < MAX_ENTITIES; i++) {
        activeEntities[i] = false;
        componentMasks[i] = 0;
    }
} 