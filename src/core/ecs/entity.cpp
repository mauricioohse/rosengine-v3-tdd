#include "entity.h"
#include <stdio.h>
#include <unordered_set>

// Initialize static members if any are needed later
EntityManager g_EntityManager;
std::unordered_set<std::type_index> componentTypes[MAX_ENTITIES];

EntityID EntityManager::CreateEntity() {
    if (entityCount >= MAX_ENTITIES) {
        printf("Warning: Reached maximum entity count!\n");
        return INVALID_ENTITY;
    }
    for (EntityID i = 1; i < MAX_ENTITIES; i++) {
        if (!activeEntities[i]) {
            activeEntities[i] = true;
            componentTypes[i].clear(); // Ensure no leftover components
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
    componentTypes[entity].clear(); // Remove all component types
    entityCount--;
}

bool EntityManager::IsEntityValid(EntityID entity) { 

    return (entity > INVALID_ENTITY && 
            entity < MAX_ENTITIES && 
            activeEntities[entity]);
}

void EntityManager::AddComponentToEntity(EntityID entity, std::type_index typeIdx) {
    if (!IsEntityValid(entity)) {
        printf("Warning: Attempting to add component to invalid entity %u\n", entity);
        return;
    }
    componentTypes[entity].insert(typeIdx);
}

void EntityManager::RemoveComponentFromEntity(EntityID entity, std::type_index typeIdx) {
    if (!IsEntityValid(entity)) {
        printf("Warning: Attempting to remove component from invalid entity %u\n", entity);
        return;
    }
    componentTypes[entity].erase(typeIdx);
    if (componentTypes[entity].empty()) {
        DestroyEntity(entity);
    }
}

bool EntityManager::HasComponent(EntityID entity, std::type_index typeIdx) {
    if (!IsEntityValid(entity)) {
        return false;
    }
    return componentTypes[entity].find(typeIdx) != componentTypes[entity].end();
}

void EntityManager::Init() {
    entityCount = 0;
    for (EntityID i = 0; i < MAX_ENTITIES; i++) {
        activeEntities[i] = false;
        componentTypes[i].clear(); // Clear all component type sets
    }
}