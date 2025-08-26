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

float ApplySlow(EntityID entity, EnemyComponent* enemy, float baseSpeed, float deltaTime) {
    if (auto* slow = Get<SlowComponent>(entity)) {
        slow->duration -= deltaTime;
        return baseSpeed * slow->intensity;
    }
    return baseSpeed;
}

bool HandlePathEnd(EntityID entity, EnemyComponent* enemy) {
    if (enemy->currPathIdx >= Grid::GetMonsterPathSize()) {
        printf("Enemy %d reached end of path, marking for destruction\n", entity);
        enemy->currHealth = -1;
        playerLife_decrease_health(1);
        return true;
    }
    return false;
}

void MoveNormalEnemy(EntityID entity, TransformComponent* transform, EnemyComponent* enemy, float speed, float deltaTime) {
    if (enemy->currPathIdx >= Grid::GetMonsterPathSize()) return;

    Point targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx + 1);

    // calculate distance to current target
    float dx = targetPos.x - transform->x;
    float dy = targetPos.y - transform->y;
    float distance = sqrt(dx*dx + dy*dy);

    // if close enough to target, advance to next path point
    if (distance <= 10.0f) {
        enemy->currPathIdx++;
               
        if (HandlePathEnd(entity, enemy)) return;
        
        targetPos = Grid::GetMonsterPathPoint(enemy->currPathIdx);
        dx = targetPos.x - transform->x;
        dy = targetPos.y - transform->y;
        distance = sqrt(dx*dx + dy*dy);
    }

    // move towards current target
    if (distance > 1.0f) {
        // pixels per second
        dx /= distance;
        dy /= distance;

        transform->x += dx * speed * deltaTime;
        transform->y += dy * speed * deltaTime;
    }
}

void MoveDebugEnemy(TransformComponent* transform, EnemyComponent* enemy, float speed, float deltaTime) {
    float oldY = transform->y;
    transform->y -= speed * deltaTime;
    printf("Debug enemy moved from y=%.1f to y=%.1f\n", oldY, transform->y);
}

bool HandleDeath(EntityID entity, EnemyComponent* enemy, EnemyDebugComponent* debug) {
    if (HasComponent<EnemyDebugComponent>(entity)) {
        // Debug enemies: die if health <= 0
        if (enemy->currHealth <= 0) {
            g_Game.debugTowerKills[debug->tower]++;
            g_Engine.entityManager.DestroyEntity(entity);
            return true;
        }
    } else {
        // Normal enemies: die if marked as not alive OR health <= 0
        if (!enemy->alive || enemy->currHealth <= 0) {
            printf("Enemy %d being destroyed: alive=%d, health=%d\n", entity, enemy->alive, enemy->currHealth);
            g_Engine.entityManager.DestroyEntity(entity);
            return true;
        }
    }
    return false;
}

bool CheckExitCollisions(EntityID entity, TransformComponent* transform, ColliderComponent* coll, std::vector<EntityID>& exits) {
    for (EntityID exit : exits) {
        if (HasComponent<TransformComponent, ColliderComponent, EnemyExitComponent>(exit)) {
            auto* exitTransform = Get<TransformComponent>(exit);
            auto* exitCollider  = Get<ColliderComponent>(exit);
            float penX, penY;

            if (exitTransform && exitCollider && CheckCollision(transform, coll, exitTransform, exitCollider, penX, penY)) {
                printf("enemy %d reached exit\n", entity);
                g_Engine.entityManager.DestroyEntity(entity);
                playerLife_decrease_health(1);
                return true;
            }
        }
    }
    return false;
}

void enemy_system::Update(float deltaTime, std::vector<EntityID> entities)
{
    // Iterate over all entities passed by the system manager
    for (EntityID entity : entities) {
        // Check if this entity has the required components
        if (!HasComponent<TransformComponent, EnemyComponent, ColliderComponent>(entity)) {
            continue;
        }

        TransformComponent* transform = Get<TransformComponent>(entity);
        EnemyComponent* enemy = Get<EnemyComponent>(entity);
        ColliderComponent* coll = Get<ColliderComponent>(entity);

        if (!transform || !enemy || !coll) {
            continue;
        }

        float speed = ApplySlow(entity, enemy, enemy->speed, deltaTime);

        EnemyDebugComponent* debug = nullptr;

        if (HasComponent<EnemyDebugComponent>(entity)) {
            debug = Get<EnemyDebugComponent>(entity);
            MoveDebugEnemy(transform, enemy, speed, deltaTime);
        }
        else {
            MoveNormalEnemy(entity, transform, enemy, speed, deltaTime);
        }

        // centralize destruction checks
        if (HandleDeath(entity, enemy, debug)) continue;
        if (CheckExitCollisions(entity, transform, coll, entities)) continue;
    }
}

void enemy_system::Destroy()
{
}
