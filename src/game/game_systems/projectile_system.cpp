#include "projectile_system.h"
#include "engine.h"
#include "math.h"
#include "main_game_scene.h"
#include "utils.h"

void projectile_system::Init()
{


}

void projectile_system::CreateProjectile()
{
}

void projectile_system::Destroy()
{
}

static void CauseExplosionDamage(TransformComponent * trf, ProjectileComponent * prt , std::vector<EntityID> entities)
{
    // iterate through all entities to find enemies within explosion radius
    for (EntityID enemy : entities) {
        if (g_Engine.entityManager.HasComponent(enemy, COMPONENT_ENEMY | COMPONENT_TRANSFORM | COMPONENT_COLLIDER)) {
            TransformComponent* enemy_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(enemy, COMPONENT_TRANSFORM);
            EnemyComponent* enemy_comp = (EnemyComponent*)g_Engine.componentArrays.GetComponentData(enemy, COMPONENT_ENEMY);
            
            // calculate distance between explosion center and enemy
            float dx = trf->x - enemy_transform->x;
            float dy = trf->y - enemy_transform->y;
            float distance = sqrt(dx * dx + dy * dy);
            
            // check if enemy is within explosion radius
            if (distance <= prt->explosionRadius && enemy_comp->alive) {
                // kill enemy by setting alive to 0
                enemy_comp->currHealth -= prt->damage;
                printf("enemy killed by explosion at distance %.2f\n", distance);
            }
        }
    }
}

void static CastJetAtTarget(int srcX, int srcY, int  destX, int destY)
{
    EntityID jet = g_mainGame.RegisterEntity();

    ADD_JET(jet, srcX, srcY, destX, destY);
    ADD_LIFETIME(jet, .2);

}

void projectile_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays * components)
{

    for (int i = 0; i < entities.size(); i++) {
        EntityID entity = entities[i];

        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_PROJECTILE | COMPONENT_TRANSFORM))
        {
            ProjectileComponent *projectile = (ProjectileComponent *)components->GetComponentData(entity, COMPONENT_PROJECTILE);
            TransformComponent *transform = (TransformComponent *)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            bool hasMoveToTarget = g_Engine.entityManager.HasComponent(entity, COMPONENT_MOVETOXY);
            bool hasReachedTarget = 0;

            if (hasMoveToTarget)
            {
                MoveToTargetXYComponent *moveToXY = (MoveToTargetXYComponent *)components->GetComponentData(entity, COMPONENT_MOVETOXY);
                float dx = transform->x - moveToXY->targetX;
                float dy = transform->y - moveToXY->targetY;
                float distance = sqrt(dx * dx + dy * dy);
                hasReachedTarget = (distance <= 10); // consider 10 pixel as tolerance
                
            }

            if (projectile->type ==  PROJECTILE_JET)
            {
                // insta cast the JET, deal damage to the enemy, and delete the projectile entity

                CastJetAtTarget(transform->x, transform->y, projectile->targetX, projectile->targetY);

                EnemyComponent * enemy = (EnemyComponent *) g_Engine.componentArrays.GetComponentData(projectile->targetEntity, COMPONENT_ENEMY);
                if (enemy)
                {
                    enemy->currHealth-=projectile->damage;
                }
                g_mainGame.DeleteEntity(entity);
                continue; // continue the for loop
            }


            if (hasReachedTarget && projectile->type == PROJECTILE_BOMB)
            {
                // create explosion component if projectile should explode
                if (projectile->shouldExplode) {
                    // only add timed sprite if it doesn't already exist
                    if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_TIMEDSPRITE)) {
                        EntityID explosion = g_mainGame.RegisterEntity();
                        ADD_TIMEDSPRITE(explosion, 0, 0.2f , 0, 3);
                        ADD_LIFETIME(explosion, 0.6f);
                        ADD_TRANSFORM(explosion, transform->x, transform->y, transform->rotation, 1.0f);
                        TimedSpriteComponent* timedSprite = (TimedSpriteComponent*)components->GetComponentData(explosion, COMPONENT_TIMEDSPRITE);
                        if (timedSprite) {
                            timedSprite->sprites[0] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_1);
                            timedSprite->sprites[1] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_2);
                            timedSprite->sprites[2] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_3);
                        }

                        CauseExplosionDamage(transform, projectile, entities);
                        
                    }
                }

                g_mainGame.DeleteEntity(entity);
            }

            if (projectile->type == PROJECTILE_PELLET)
            {
                // check collision with enemies
                ColliderComponent* projectileCollider = (ColliderComponent*)components->GetComponentData(entity, COMPONENT_COLLIDER);
                if (!projectileCollider) {
                    static bool errorPrinted = false;
                    if (!errorPrinted) {
                        printf("ERROR: pellet projectile missing collider component\n");
                        errorPrinted = true;
                    }
                    continue;
                }
                
                for (EntityID otherEntity : entities) {
                    if (otherEntity == entity) continue;
                    
                    if (g_Engine.entityManager.HasComponent(otherEntity, COMPONENT_ENEMY | COMPONENT_COLLIDER | COMPONENT_TRANSFORM)) {
                        ColliderComponent* enemyCollider = (ColliderComponent*)components->GetComponentData(otherEntity, COMPONENT_COLLIDER);
                        TransformComponent* enemyTransform = (TransformComponent*)components->GetComponentData(otherEntity, COMPONENT_TRANSFORM);
                        
                        if (!enemyCollider) {
                            static bool enemyErrorPrinted = false;
                            if (!enemyErrorPrinted) {
                                printf("ERROR: enemy missing collider component\n");
                                enemyErrorPrinted = true;
                            }
                            continue;
                        }
                        
                        float penX, penY;
                        if (CheckCollisionCentered(transform, projectileCollider, enemyTransform, enemyCollider, penX, penY)) {
                            EnemyComponent* enemy = (EnemyComponent*)components->GetComponentData(otherEntity, COMPONENT_ENEMY);
                            if (enemy) {
                                enemy->currHealth -= projectile->damage;
                            }
                            g_mainGame.DeleteEntity(entity);
                            break;
                        }
                    }
                }
            }

            if (projectile->type == PROJECTILE_GUST)
            {
                // creates a CC entity with gust sprite. delete the projectile, leave the CC entity to deal with the enemy.
                EntityID ccEntity = g_mainGame.RegisterEntity();
                
                ADD_TRANSFORM(ccEntity, projectile->targetX, projectile->targetY, 0.0f, 1.0f);
                ADD_CC(ccEntity, projectile->targetEntity, projectile->targetX, projectile->targetY);
                ADD_LIFETIME(ccEntity, 1.0f);
                ADD_TIMEDSPRITE(ccEntity, .8f, .2f, 1, 4);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[0] = ResourceManager::GetTexture(TEXTURE_GUST_1);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[1] = ResourceManager::GetTexture(TEXTURE_GUST_2);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[2] = ResourceManager::GetTexture(TEXTURE_GUST_3);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[3] = ResourceManager::GetTexture(TEXTURE_GUST_4);

                g_mainGame.DeleteEntity(entity);
            }
        }
    
    
    
        // deals with the CC entities. This could be its own system, but it is not for the timebeing.
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CC))
        {
            CrowdcontrolComponent* cc = (CrowdcontrolComponent*)components->GetComponentData(entity, COMPONENT_CC);
            if (cc && cc->target != INVALID_ENTITY) {
                // check if cc entity has lifetime component
                if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_LIFETIME)) {
                    static bool ccLifetimeErrorPrinted = false;
                    if (!ccLifetimeErrorPrinted) {
                        printf("ERROR: cc entity missing lifetime component\n");
                        ccLifetimeErrorPrinted = true;
                    }
                }
                
                // freeze enemy at its position by removing movement components
                if (g_Engine.entityManager.HasComponent(cc->target, COMPONENT_TRANSFORM)) {
                    TransformComponent* targetTransform = (TransformComponent*)components->GetComponentData(cc->target, COMPONENT_TRANSFORM);
                    if (targetTransform) {
                        targetTransform->x = cc->targetX;
                        targetTransform->y = cc->targetY;
                    }
                }
            }
        }
    }
}
