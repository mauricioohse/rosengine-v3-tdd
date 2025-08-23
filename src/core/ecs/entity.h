#pragma once
#include "ecs_types.h"
#include <typeindex>

struct EntityManager {
    // Tracks which entities are active
    bool activeEntities[MAX_ENTITIES];
    // Number of active entities
    uint32_t entityCount;
    
    // Core functions
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    bool IsEntityValid(EntityID entity);
    
    // Component relationship functions
    void AddComponentToEntity(EntityID entity, std::type_index typeIdx);
    void RemoveComponentFromEntity(EntityID entity, std::type_index typeIdx);
    bool HasComponent(EntityID entity, std::type_index typeIdx);
    void Init();
};
