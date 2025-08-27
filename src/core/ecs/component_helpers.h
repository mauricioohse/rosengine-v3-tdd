#pragma once

// Forward declaration
struct SceneBase;

// ===== CONVENIENCE FUNCTIONS =====

// ForEachComponent usage example:
//
// ForEachComponent<TowerComponent, TransformComponent>(scene, [&](auto entity, auto* tower, auto* transform) {
//     // Do something with each entity of the scene that contains tower and transform components
// });
//
// Note: You can add "return false" to break the ForEachComponent loop early or "return true" to skip to the next ForEachComponent iteration.
// Note: If you don't add a return, the loop will continue without any intervention until it iterates over all scene entities.
// Important: Adding any return statement to the callback function will require return statements at all exit points of that callback function.

template<typename Comp, typename Func>
void ForEachComponent(SceneBase* scene, Func&& callback);

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
void Remove(EntityID entity);

#include "component_helpers.tpp"
