#include "scene_manager.h"
#include "engine.h"

EntityID SceneBase::RegisterEntity(){
    EntityID newEntity = g_Engine.entityManager.CreateEntity();

    entities.push_back(newEntity);

    return newEntity;
}

void SceneBase::DeleteEntity(EntityID entity){
    // find the entity in our vector
    for(int i = 0; i < entities.size(); i++){
        if(entities[i] == entity){
            // remove from our scene's entity list
            // TODO: this changes the entity vector order... can cause issues!
            entities[i] = entities[entities.size() - 1];
            entities.pop_back();
            
            // destroy the entity in the entity manager
            g_Engine.entityManager.DestroyEntity(entity);
            return;
        }
    }
    
    // entity not found in this scene
    printf("warning: attempted to delete entity %d not found in scene %s\n", entity, name);
}

void SceneBase::CleanDeletedEntities(){

    for (EntityID e : entities){
        if (!g_Engine.entityManager.IsEntityValid(e)){
            DeleteEntity(e);
        }
    }

}

void SceneBase::OnUpdate(float deltaTime) {
    // Call the optional update first
    OptionalUpdate(deltaTime);
    
    // Then do the standard system updates
    g_Engine.systemManager.UpdateSystems(deltaTime, entities, &g_Engine.componentArrays);

    CleanDeletedEntities();
}

void SceneManager::Init() {
    
}
void SceneManager::Update(float deltaTime) {

    for (int i = 0; sceneStack[i]!=nullptr;i++)
    {
        SceneBase* scene = sceneStack[i];
        switch (scene->state){
            case SceneState::ACTIVE:
                scene->OnUpdate(deltaTime);   
                break;

            case SceneState::PAUSED:
                // scene->OnPause(deltaTime);
                break;
            
            default:
                break;
        }

    }

}


void SceneManager::Cleanup() {
    memset(this, 0, sizeof(this));
}

// stack operations
void SceneManager::PushScene(SceneBase *scene) {
    if (stackSize < MAX_SCENE_STACK_SIZE) {
        sceneStack[stackSize++] = scene;
    } else {
        printf("ERROR: scene stack overflow - cannot push new scene\n");
    }

}    // add scene to top of stack
void SceneManager::PopScene() {
    free(sceneStack[stackSize--]);
}                     


