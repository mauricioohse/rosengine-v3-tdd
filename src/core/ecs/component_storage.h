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
};

template<typename T>
class ComponentArray : public IComponentArray {
    std::vector<T> componentsArray;
public:
    ComponentArray(size_t size) : componentsArray(size) {}
    void* GetData(EntityID entity) override {
        return &componentsArray[entity];
    }
};

class ComponentStorage {
public:
    // static ComponentStorage& Instance();

    /// @brief Register a new component type
    template<typename T>
    static void Register() {
        printf("Registering component: %s\n", typeid(T).name());
        customArrayNameSoNoDuplicateExists[TypeIndex<T>()] = std::make_unique<ComponentArray<T>>(MAX_ENTITIES);
    }

    /// @brief Get the component array for a specific type
    template<typename T>
    static ComponentArray<T>* GetArray() {
        auto it = customArrayNameSoNoDuplicateExists.find(TypeIndex<T>());
        if (it != customArrayNameSoNoDuplicateExists.end()) {
            return static_cast<ComponentArray<T>*>(it->second.get());
        }
        return nullptr;
    }

private:

    // ComponentStorage() = default;

    // static ComponentStorage* instance;

    static std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> customArrayNameSoNoDuplicateExists;
};
