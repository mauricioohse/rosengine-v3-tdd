#include "enemy_system.h"
#include "game.h"
#include "grid.h"
#include "math.h"
#include "../../core/utils.h"

void enemy_system::Init()
{
}

void enemy_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    
    // find all entities with transform + enemy
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, C_Transform | C_Enemy | C_Collider)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, C_Transform);

            EnemyComponent *enemy =
                (EnemyComponent *)components->GetComponentData(entity, C_Enemy);

            ColliderComponent *coll =
                (ColliderComponent *)components->GetComponentData(entity, C_Collider);

            EnemyDebugComponent * debug = 
                (EnemyDebugComponent*)components->GetComponentData(entity, C_EnemyDebug);    

            float speed = 50.0f;
            
            if (transform && enemy && !debug)
            {
                // get current target position from monster path
                if (enemy->currPathIdx < Grid::GetMonsterPathSize()) {
                    Point targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);
                    
                    // calculate distance to current target
                    float dx = targetPos.x - transform->x;
                    float dy = targetPos.y - transform->y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    // if close enough to target, advance to next path point
                    if (distance <= 10.0f) {
                        enemy->currPathIdx++;
                        if (enemy->currPathIdx >= Grid::GetMonsterPathSize()) {
                            // reached end of path, destroy enemy
                            g_Engine.entityManager.DestroyEntity(entity);
                            continue;
                        }
                        // get new target position
                        targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);
                        dx = targetPos.x - transform->x;
                        dy = targetPos.y - transform->y;
                        distance = sqrt(dx * dx + dy * dy);
                    }
                    
                    // move towards current target
                    if (distance > 1.0f) {
                         // pixels per second
                        dx /= distance; // normalize
                        dy /= distance;
                        
                        transform->x += dx * speed * deltaTime;
                        transform->y += dy * speed * deltaTime;
                    }
                }
            }
            else if( transform && enemy && debug)
            {
                // just move upward
                transform->y -= speed * deltaTime;
            }

            if (!enemy->alive)
            {
                g_Engine.entityManager.DestroyEntity(entity);
                printf("killing entity %d because note alive\n",entity);
            }

            // check if enemy health is above zero, if not, destroy him
            if (enemy && enemy->currHealth <= 0)
            {

                EnemyDebugComponent *debug =
                (EnemyDebugComponent *)g_Engine.componentArrays.GetComponentData(entity, C_EnemyDebug);

                if (debug)
                {
                    g_Game.debugTowerKills[debug->tower]++;
                }

                g_Engine.entityManager.DestroyEntity(entity);
    
            }

            // Check if enemy hit any of the exit collider entities
            for (EntityID exit : entities)
            {
                if (g_Engine.entityManager.HasComponent(exit, C_EnemyExit | C_Transform | C_Collider))
                {
                    TransformComponent* exitTransform = 
                        (TransformComponent*)components->GetComponentData(exit, C_Transform);
                    ColliderComponent* exitCollider = 
                        (ColliderComponent*)components->GetComponentData(exit, C_Collider);
                    
                    if (exitTransform && exitCollider) {
                        float penX, penY;
                        if (CheckCollisionCentered(transform, coll, exitTransform, exitCollider, penX, penY)) {
                            // enemy reached exit, destroy it
                            g_Engine.entityManager.DestroyEntity(entity);
                            printf("enemy %d reached exit\n", entity);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void enemy_system::Destroy()
{
}
