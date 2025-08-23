#pragma once

#include <memory>
#include <functional>
#include <type_traits>
#include "../engine.h"
#include "component_storage.h"
#include "component_helpers.h"

using namespace std;

template<typename ComponentType>
template<typename... Args>
void ComponentManager<ComponentType>::Add(EntityID entity, Args&&... args) {
    auto* array = ComponentStorage::GetArray<ComponentType>();

    g_Engine.entityManager.AddComponentToEntity(entity, TypeIndex<ComponentType>());

    ComponentType* component = array ? static_cast<ComponentType*>(array->GetData(entity)) : nullptr;
    if (component) {
        component->Init(forward<Args>(args)...);
    } else {
        printf("Failed to retrieve %s component for entity %u\n", typeid(ComponentType).name(), entity);
    }
}

// Get component from entity
template<typename ComponentType>
ComponentType* ComponentManager<ComponentType>::Get(EntityID entity) {
    auto* array = ComponentStorage::GetArray<ComponentType>();
    return array ? static_cast<ComponentType*>(array->GetData(entity)) : nullptr;
}

// Check if entity has component
template<typename ComponentType>
bool ComponentManager<ComponentType>::Has(EntityID entity) {
    return g_Engine.entityManager.HasComponent(entity, TypeIndex<ComponentType>());
}

// Remove component from entity
template<typename ComponentType>
void ComponentManager<ComponentType>::Remove(EntityID entity) {
    g_Engine.entityManager.RemoveComponentFromEntity(entity, TypeIndex<ComponentType>());
}

// ===== SINGLE COMPONENT ITERATION =====

// Simple iteration over entities with this component type
template<typename ComponentType>
template<typename Func>
void ComponentManager<ComponentType>::ForEach(SceneBase* scene, Func&& callback) {
    for (EntityID entity : scene->entities) {
        if (Has(entity)) {
            ComponentType* _component = Get(entity);

            if (_component)
            {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, ComponentType*>) {
                    // If the callback returns false, break early
                    // If the callback returns true, skip to next iteration
                    if (!callback(entity, _component))
                        break;
                } else {
                    // Normal callback, no early exit required
                    callback(entity, _component);
                }
            }
        }
    }
}

// ===== MULTI-COMPONENT ITERATION =====

// Iterate over entities with two component types
template<typename ComponentType>
template<typename OtherComponent, typename Func>
void ComponentManager<ComponentType>::ForEachWith(SceneBase* scene, Func&& callback) {
    for (EntityID entity : scene->entities) {
        if (HasComponents<ComponentType, OtherComponent>(entity)) {
            ComponentType* _comp1 = Get(entity);
            OtherComponent* _comp2 = ComponentManager<OtherComponent>::Get(entity);

            if (_comp1 && _comp2) {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, ComponentType*, OtherComponent*>) {
                    // If the callback returns false, break early
                    // If the callback returns true, skip to next iteration
                    if (!callback(entity, _comp1, _comp2))
                        break;
                } else {
                    // Normal callback, no early exit required
                    callback(entity, _comp1, _comp2);
                }
            }
        }
    }
}

// Iterate over entities with three component types
template<typename ComponentType>
template<typename Comp2, typename Comp3, typename Func>
void ComponentManager<ComponentType>::ForEachWith(SceneBase* scene, Func&& callback) {
    for (EntityID entity : scene->entities) {
        if (HasComponents<ComponentType, Comp2, Comp3>(entity)) {
            ComponentType* _comp1 = Get(entity);
            Comp2* _comp2 = ComponentManager<Comp2>::Get(entity);
            Comp3* _comp3 = ComponentManager<Comp3>::Get(entity);

            if (_comp1 && _comp2 && _comp3) {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, ComponentType*, Comp2*, Comp3*>) {
                    // If the callback returns false, break early
                    // If the callback returns true, skip to next iteration
                    if (!callback(entity, _comp1, _comp2, _comp3)) break;
                } else {
                    // Normal callback, no early exit required
                    callback(entity, _comp1, _comp2, _comp3);
                }
            }
        }
    }
}

// Iterate over entities with four component types
template<typename ComponentType>
template<typename Comp2, typename Comp3, typename Comp4, typename Func>
void ComponentManager<ComponentType>::ForEachWith(SceneBase* scene, Func&& callback) {
    for (EntityID entity : scene->entities) {
        if (HasComponents<ComponentType, Comp2, Comp3, Comp4>(entity)) {
            ComponentType* _comp1 = Get(entity);
            Comp2* _comp2 = ComponentManager<Comp2>::Get(entity);
            Comp3* _comp3 = ComponentManager<Comp3>::Get(entity);
            Comp4* _comp4 = ComponentManager<Comp4>::Get(entity);

            if (_comp1 && _comp2 && _comp3 && _comp4) {
                if constexpr (std::is_invocable_r_v<bool, Func, EntityID, ComponentType*, Comp2*, Comp3*, Comp4*>) {
                    // If the callback returns false, break early
                    // If the callback returns true, skip to next iteration
                    if (!callback(entity, _comp1, _comp2, _comp3, _comp4)) break;
                } else {
                    // Normal callback, no early exit required
                    callback(entity, _comp1, _comp2, _comp3, _comp4);
                }
            }
        }
    }
}

template<typename ComponentType>
bool HasComponent(EntityID entity) {
    return g_Engine.entityManager.HasComponent(entity, TypeIndex<ComponentType>());
}

// Check if an entity has all of the specified components
template<typename... ComponentTypes>
bool HasComponents(EntityID entity) {
    return (... && g_Engine.entityManager.HasComponent(entity, TypeIndex<ComponentTypes>()));
}

// Check if an entity has any of the specified components
template<typename... ComponentTypes>
bool HasAnyComponent(EntityID entity) {
    return (... || g_Engine.entityManager.HasComponent(entity, TypeIndex<ComponentTypes>()));
}