#pragma once

#include "ecs_types.h"

// Forward declaration
struct SceneBase;
// extern Engine g_Engine;

using namespace std;

// Template-based component manager
template<typename ComponentType>
class ComponentManager {
public:
    template<typename... Args>
    static void Add(EntityID entity, Args&&... args);
    
    // Get component from entity
    static ComponentType* Get(EntityID entity);

    // Check if entity has component
    static bool Has(EntityID entity);

    // Remove component from entity
    static void Remove(EntityID entity);

    // Simple iteration over entities with this component type
    template<typename Func>
    static void ForEach(SceneBase* scene, Func&& callback);

    // Iterate over entities with two component types
    template<typename OtherComponent, typename Func>
    static void ForEachWith(SceneBase* scene, Func&& callback);
    
    // Iterate over entities with three component types
    template<typename Comp2, typename Comp3, typename Func>
    static void ForEachWith(SceneBase* scene, Func&& callback);
    
    // Iterate over entities with four component types
    template<typename Comp2, typename Comp3, typename Comp4, typename Func>
    static void ForEachWith(SceneBase* scene, Func&& callback);
};

// Check if an entity has a specific component
template<typename ComponentType>
bool HasComponent(EntityID entity);

// Check if an entity has all of the specified components
template<typename... ComponentTypes>
bool HasComponents(EntityID entity);

// Check if an entity has any of the specified components
template<typename... ComponentTypes>
bool HasAnyComponent(EntityID entity);

#include "component_manager.tpp"
