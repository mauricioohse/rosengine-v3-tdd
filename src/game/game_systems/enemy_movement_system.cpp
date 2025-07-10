#include "enemy_movement_system.h"
#include "game.h"

void enemy_movement_system::Init()
{
}

void enemy_movement_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    
    // find all entities with transform + enemy
    // move them to the left 1 pixel each update call
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_ENEMY)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);

            EnemyComponent * enemy = 
                (EnemyComponent*)components->GetComponentData(entity, COMPONENT_ENEMY);

            if (transform)
            {
                transform->x -= 1.0f;
            }

            if (!enemy->alive)
            {
                g_Engine.entityManager.DestroyEntity(entity);
                printf("killing entity %d because note alive\n",entity);
            }
        }
    }
}

void enemy_movement_system::Destroy()
{
}
