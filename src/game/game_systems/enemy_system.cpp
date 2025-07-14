#include "enemy_system.h"
#include "game.h"
#include "grid.h"
#include "math.h"

void enemy_system::Init()
{
}

void enemy_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    
    // find all entities with transform + enemy
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_ENEMY)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);

            EnemyComponent * enemy = 
                (EnemyComponent*)components->GetComponentData(entity, COMPONENT_ENEMY);

            if (transform && enemy)
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
                        float speed = 50.0f; // pixels per second
                        dx /= distance; // normalize
                        dy /= distance;
                        
                        transform->x += dx * speed * deltaTime;
                        transform->y += dy * speed * deltaTime;
                    }
                }
            }

            if (!enemy->alive)
            {
                g_Engine.entityManager.DestroyEntity(entity);
                printf("killing entity %d because note alive\n",entity);
            }

            // check if enemy health is above zero, if not, destroy him
            if (enemy && enemy->currHealth <= 0)
            {
                g_Engine.entityManager.DestroyEntity(entity);
            }
        }
    }
}

void enemy_system::Destroy()
{
}
