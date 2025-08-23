#include "enemy_system.h"
#include "game.h"
#include "grid.h"
#include "math.h"
#include "../../core/utils.h"
#include "main_game_scene.h"
#include "player_life_system.h"

void enemy_system::Init()
{
}

void enemy_system::Update(float deltaTime, std::vector<EntityID> entities)
{
    SceneBase* scene = &g_mainGame;
    // FOR_EACH_COMPONENT_3(scene, entity,
    //                      Transform, transform,
    //                      Enemy, enemy,
    //                      Collider, coll)
    ForEachComponent<TransformComponent, EnemyComponent, ColliderComponent>(scene, [&](EntityID entity, TransformComponent* transform, EnemyComponent* enemy, ColliderComponent* coll) {
        printf("Processing enemy %d\n", entity);
        float speed = enemy->speed;

        SlowComponent* enemy_slow = Get<SlowComponent>(entity);
        if (enemy_slow)
        {
            speed *= enemy_slow->intensity;
            enemy_slow->duration -= deltaTime;
        }

        EnemyDebugComponent* debug = Get<EnemyDebugComponent>(entity);

        if (transform && enemy && !debug)
        {
            // get current target position from monster path
            if (enemy->currPathIdx < Grid::GetMonsterPathSize())
            {
                Point targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);

                // calculate distance to current target
                float dx = targetPos.x - transform->x;
                float dy = targetPos.y - transform->y;
                printf("Enemy %d: dx=%f, dy=%f, about to call sqrt\n", entity, dx, dy);
                float distance = sqrt(dx * dx + dy * dy);
                printf("Enemy %d: distance=%f\n", entity, distance);

                // if close enough to target, advance to next path point
                if (distance <= 10.0f)
                {
                    enemy->currPathIdx++;
                    if (enemy->currPathIdx >= Grid::GetMonsterPathSize())
                    {
                        // reached end of path, mark for destruction
                        printf("Enemy %d reached end of path, marking for destruction\n", entity);
                        enemy->currHealth = -1; // mark as dead
                        playerLife_decrease_health(1);
                        return true; // skips the ForEachComponent loop to the next iteration
                    }
                    // get new target position
                    targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);
                    dx = targetPos.x - transform->x;
                    dy = targetPos.y - transform->y;
                    distance = sqrt(dx * dx + dy * dy);
                }

                // move towards current target
                if (distance > 1.0f)
                {
                    // pixels per second
                    dx /= distance; // normalize
                    dy /= distance;

                    transform->x += dx * speed * deltaTime;
                    transform->y += dy * speed * deltaTime;
                }
            }
        }
        else if (transform && enemy && debug)
        {
            // just move upward
            transform->y -= speed * deltaTime;
        }

        if (!enemy->alive)
        {
            g_Engine.entityManager.DestroyEntity(entity);
            printf("killing entity %d because note alive\n", entity);
        }

        // check if enemy health is above zero, if not, destroy him
        if (enemy && enemy->currHealth <= 0)
        {
            EnemyDebugComponent* debug = Get<EnemyDebugComponent>(entity);

            if (debug)
            {
                g_Game.debugTowerKills[debug->tower]++;
            }

            g_Engine.entityManager.DestroyEntity(entity);
        }

        // Check if enemy hit any of the exit collider entities
        for (EntityID exit : entities)
        {
            if (HasComponents<TransformComponent, ColliderComponent, EnemyExitComponent>(exit))
            {
                TransformComponent *exitTransform = Get<TransformComponent>(exit);
                ColliderComponent *exitCollider = Get<ColliderComponent>(exit);

                if (exitTransform && exitCollider)
                {
                    float penX, penY;
                    if (CheckCollision(transform, coll, exitTransform, exitCollider, penX, penY))
                    {
                        // enemy reached exit, destroy it
                        g_Engine.entityManager.DestroyEntity(entity);
                        printf("enemy %d reached exit\n", entity);
                        playerLife_decrease_health(1);
                        return false; // breaks the ForEachComponent loop
                    }
                }
            }
        } 
        
        // check if enemy is dead and should be destroyed
        if (enemy->currHealth <= 0) {
            printf("Destroying dead enemy %d\n", entity);
            g_Engine.entityManager.DestroyEntity(entity);
            return true; // skip to next iteration
        }
        
        return false; // continue processing
    });
}

void enemy_system::Destroy()
{
}
