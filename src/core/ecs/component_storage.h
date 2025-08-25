#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <cstdio>
#include "ecs_types.h"

// Forward declaration
struct SceneBase;

class IComponentArray {
public:
    virtual void* GetData(EntityID entity) = 0;
    virtual ~IComponentArray() = default;
    virtual void Remove(EntityID entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
    std::vector<T> componentsArray;
public:
    ComponentArray(size_t size) : componentsArray(size) {}
    void* GetData(EntityID entity) override {
        return &componentsArray[entity];
    }
    void Remove(EntityID entity) override {
        componentsArray[entity] = T{};  // reset to default
    }
};

class ComponentStorage {
public:
    /// @brief Register a new component type
    template<typename T>
    static void Register() {
        printf("Registering component: %s\n", typeid(T).name());
        componentTypesArray[TypeIndex<T>()] = std::make_unique<ComponentArray<T>>(MAX_ENTITIES);
    }

    /// @brief Get the component array for a specific type
    template<typename T>
    static ComponentArray<T>* GetArray() {
        auto it = componentTypesArray.find(TypeIndex<T>());
        if (it != componentTypesArray.end()) {
            return static_cast<ComponentArray<T>*>(it->second.get());
        }
        return nullptr;
    }

    static void RemoveByType(EntityID entity, std::type_index type) {
        auto it = componentTypesArray.find(type);
        if (it != componentTypesArray.end()) {
            it->second->Remove(entity);
        }
    }

private:

    static std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentTypesArray;
};
