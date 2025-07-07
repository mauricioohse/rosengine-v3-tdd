#pragma once
#include "ecs/ecs_types.h"
#include "core/ecs/entity.h"
#include "vector"

#define MAX_SCENE_STACK_SIZE 100

enum struct SceneState {
    ACTIVE,    // fully running, visible
    PAUSED,    // visible but not updating (for UI overlays)
    INACTIVE,  // not visible, not updating, but preserved
};

struct SceneBase {
    // scene identification
    const char* name;
    SceneState state;
    
    // EntityID *sceneEntities; // IDs owned by scene
    // uint32_t entityCount;

    // TODO: use a better data structure than vector
    // TODO: passing std::vector<EntityID> actually causes a copy of the whole entities array to the function. that should not be done.
    std::vector<EntityID> entities;

    
    EntityID RegisterEntity();
    void DeleteEntity(EntityID entity);
    
    // virtual functions for scene lifecycle
    virtual void OnLoad() = 0;    // called when scene is first loaded
    virtual void OptionalUpdate(float deltaTime) {} // Optional update function for derived classes
    void OnUpdate(float deltaTime);  // called every frame when scene is active

    void CleanDeletedEntities();
};

struct SceneManager {
    SceneBase* sceneStack[MAX_SCENE_STACK_SIZE];
    uint32_t stackSize;
    
    // core functions
    void Init();
    void Update(float deltaTime);
    void Cleanup();
    
    // stack operations
    void PushScene(SceneBase* scene);    // add scene to top of stack
    void PopScene();                     // remove top scene
    void ReplaceScene(SceneBase* scene); // pop current and push new
    
    // scene state management
    void SetSceneState(SceneBase* scene, SceneState state);
    SceneBase* GetActiveScene();
    SceneBase* GetSceneByName(const char* name);
    
    // utility functions
    bool IsSceneInStack(SceneBase* scene);
    uint32_t GetSceneCount();
};

extern SceneManager g_SceneManager;  // global scene manager instance