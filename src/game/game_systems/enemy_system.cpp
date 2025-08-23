#include "enemy_system.h"
#include "game.h"
#include "grid.h"
#include "math.h"
#include "../../core/utils.h"
#include "main_game_scene.h"
#include "player_life_system.h"
#include "enemy_spawner.h"

void enemy_system::Init()
{
}

void enemy_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    FOR_EACH_COMPONENT_3((&g_mainGame), entity,
                         Transform, transform,
                         Enemy, enemy,
                         Collider, coll)
    {
        float speed = enemy->speed;

        auto enemy_slow = GET_Slow(entity);
        if (enemy_slow)
        {
            speed *= enemy_slow->intensity;
            enemy_slow->duration -= deltaTime;
        }

        EnemyDebugComponent * debug = GET_EnemyDebug(entity);

        if (transform && enemy && !debug)
        {
            // get current target position from monster path
            if (enemy->currPathIdx < Grid::GetMonsterPathSize())
            {
                Point targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);

                // calculate distance to current target
                float dx = targetPos.x - transform->x;
                float dy = targetPos.y - transform->y;
                float distance = sqrt(dx * dx + dy * dy);

                // if close enough to target, advance to next path point
                if (distance <= 10.0f)
                {
                    enemy->currPathIdx++;
                    if (enemy->currPathIdx >= Grid::GetMonsterPathSize())
                    {
                        // reached end of path, destroy enemy
                        g_Engine.entityManager.DestroyEntity(entity);
                        playerLife_decrease_health(1);
                        continue;
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
        
            // specific enemy logic here
            switch (enemy->type)
            {
                // TODO: in the future, if spawning enemies is not only enemy_boss specific, we should refactor this to somewhere else
                case ENEMY_BOSS:
                {
                    // first, check if enemySpawner component already exists. if not, create it.
                    EnemySpawnerComponent *spawner = GET_EnemySpawner(entity);

                    if (!spawner)
                    {
                        // if the entity does not have the spawner component yet, we add it here once
                        spawner = ADD_EnemySpawner(entity, 3.0f, ENEMY_RUNNER_III);
                    }

                    // if already exists, then apply the enemySpawn when the cooldown is less than zero, and also reset the cooldown
                    if (spawner)
                    {
                        spawner->currentCooldown -= deltaTime;

                        if (spawner->currentCooldown <= 0.0f)
                        {
                            // spawn enemy near the boss
                            Point bossPos = {(int)transform->x, (int)transform->y};
                            int spawnX = bossPos.x;
                            int spawnY = bossPos.y;

                            EntityID spawnedEnemy = EnemySpawner::SpawnEnemyAt(&g_mainGame, spawnX, spawnY, (ENEMY_TYPE)spawner->spawnType);
                            auto spawnedEnemy_enemyComponent = GET_Enemy(spawnedEnemy);
                            spawnedEnemy_enemyComponent->currPathIdx = enemy->currPathIdx;

                            spawner->currentSpawns++;
                            spawner->currentCooldown = spawner->spawnCooldown;

                            printf("boss spawned minion %d/%d at (%d, %d)\n",
                                spawner->currentSpawns, spawnX, spawnY);
                        }
                    }
                }
                break;

            default:
                // no specific behavior for other enemy types
                break;
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

            EnemyDebugComponent *debug =
                (EnemyDebugComponent *)GET_COMPONENT(entity, C_EnemyDebug);

            if (debug)
            {
                g_Game.debugTowerKills[debug->element]++;
            }

            g_Engine.entityManager.DestroyEntity(entity);
        }

        // Check if enemy hit any of the exit collider entities
        for (EntityID exit : entities)
        {
            if (HAS_COMPONENT(exit, C_EnemyExit | C_Transform | C_Collider))
            {
                TransformComponent *exitTransform =
                    (TransformComponent *)components->GetComponentData(exit, C_Transform);
                ColliderComponent *exitCollider =
                    (ColliderComponent *)components->GetComponentData(exit, C_Collider);

                if (exitTransform && exitCollider)
                {
                    float penX, penY;
                    if (CheckCollision(transform, coll, exitTransform, exitCollider, penX, penY))
                    {
                        // enemy reached exit, destroy it
                        g_Engine.entityManager.DestroyEntity(entity);
                        printf("enemy %d reached exit\n", entity);
                        playerLife_decrease_health(1);
                        break;
                    }
                }
            }
        } 
    } END_FOR_EACH
}

void enemy_system::Destroy()
{
}
