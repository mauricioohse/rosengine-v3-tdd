#pragma once
#include "ecs_types.h"

struct EntityManager {
    // Tracks which components each entity has
    uint32_t componentMasks[MAX_ENTITIES];
    // Tracks which entities are active
    bool activeEntities[MAX_ENTITIES];
    // Number of active entities
    uint32_t entityCount;
    
    // Core functions
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    bool IsEntityValid(EntityID entity);
    
    // Component relationship functions
    void AddComponentToEntity(EntityID entity, ComponentType type);
    void RemoveComponentFromEntity(EntityID entity, ComponentType type);
    bool HasComponent(EntityID entity, ComponentType type);
    void Init();
};
