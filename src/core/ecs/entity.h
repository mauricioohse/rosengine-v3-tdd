#pragma once
#include "ecs_types.h"
#include <typeindex>
#include <vector>

struct EntityManager {
    // Tracks which components each entity has
    uint64_t componentMasks[MAX_ENTITIES];
    // Tracks which entities are active
    bool activeEntities[MAX_ENTITIES];
    // Number of active entities
    uint32_t entityCount;
    
    // Core functions
    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    bool IsEntityValid(EntityID entity);
    void ProcessPendingDestructions();
    
    // Component relationship functions
    void AddComponentToEntity(EntityID entity, std::type_index typeIdx);
    void RemoveComponentFromEntity(EntityID entity, std::type_index typeIdx);
    bool HasComponent(EntityID entity, std::type_index typeIdx);
    void Init();

private:
    std::vector<EntityID> toDestroy;
};
