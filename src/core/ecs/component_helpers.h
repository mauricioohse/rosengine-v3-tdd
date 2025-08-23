#pragma once

// Forward declaration
struct SceneBase;

// ===== CONVENIENCE FUNCTIONS =====

// These provide a cleaner interface for multi-component iteration

/// @brief Iterates on the entities of a scene
template<typename Comp, typename Func>
void ForEachComponent(SceneBase* scene, Func&& callback);

/// @brief Loop over the entities of a scene that have the specified components
/// @return add "return false" to break the loop, "return true" to skip to next iteration, no return continues with no intervention
/// @code
/// ForEachComponent<TowerComponent, TransformComponent>(scene, [&](auto entity, auto* tower, auto* transform) {
///     // Do something with tower and transform
/// });
/// @endcode
template<typename Comp1, typename Comp2, typename Func>
void ForEachComponent(SceneBase* scene, Func&& callback);

template<typename Comp1, typename Comp2, typename Comp3, typename Func>
void ForEachComponent(SceneBase* scene, Func&& callback);

template<typename Comp1, typename Comp2, typename Comp3, typename Comp4, typename Func>
void ForEachComponent(SceneBase* scene, Func&& callback);

// Type-safe component access
template<typename ComponentType>
ComponentType* Get(EntityID entity);

template<typename ComponentType>
void RemoveComponent(EntityID entity);

#include "component_helpers.tpp"