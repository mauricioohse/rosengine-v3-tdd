#include "projectile_system.h"
#include "engine.h"
#include "math.h"
#include "main_game_scene.h"
#include "utils.h"
#include "play_sound.h"


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
        if (g_Engine.entityManager.HasComponent(enemy, C_Enemy | C_Transform | C_Collider)) {
            TransformComponent* enemy_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(enemy, C_Transform);
            EnemyComponent* enemy_comp = (EnemyComponent*)g_Engine.componentArrays.GetComponentData(enemy, C_Enemy);
            
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

    PlaySound::PlaySound(SOUND_BOOM_LOW);
}

void static CastJetAtTarget(int srcX, int srcY, int  destX, int destY)
{
    EntityID jet = g_mainGame.RegisterEntity();

    ADD_JetAnimation(jet, srcX, srcY, destX, destY);
    ADD_LifeTime(jet, .2);

}

void projectile_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays * components)
{

    for (int i = 0; i < entities.size(); i++) {
        EntityID entity = entities[i];

        if (g_Engine.entityManager.HasComponent(entity, C_Projectile | C_Transform))
        {
            ProjectileComponent *projectile = (ProjectileComponent *)components->GetComponentData(entity, C_Projectile);
            TransformComponent *transform = (TransformComponent *)components->GetComponentData(entity, C_Transform);
            bool hasMoveToTarget = g_Engine.entityManager.HasComponent(entity, C_MoveToXY);
            bool hasReachedTarget = 0;

            if (hasMoveToTarget)
            {
                MoveToXYComponent *moveToXY = (MoveToXYComponent *)components->GetComponentData(entity, C_MoveToXY);
                float dx = transform->x - moveToXY->targetX;
                float dy = transform->y - moveToXY->targetY;
                float distance = sqrt(dx * dx + dy * dy);
                hasReachedTarget = (distance <= 10); // consider 10 pixel as tolerance
                
            }

            if (projectile->type ==  PROJECTILE_JET)
            {
                // insta cast the JET, deal damage to the enemy, and delete the projectile entity

                CastJetAtTarget(transform->x, transform->y, projectile->targetX, projectile->targetY);
                PlaySound::PlaySound(SOUND_SHOOT_LOW1);

                
                EnemyComponent * enemy = (EnemyComponent *) g_Engine.componentArrays.GetComponentData(projectile->targetEntity, C_Enemy);
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
                    if (!g_Engine.entityManager.HasComponent(entity, C_TimedSprite)) {
                        EntityID explosion = g_mainGame.RegisterEntity();
                        ADD_TimedSprite(explosion, 0, 0.2f , 0, 3);
                        ADD_LifeTime(explosion, 0.6f);
                        ADD_Transform(explosion, transform->x, transform->y, transform->rotation, 1.0f);
                        TimedSpriteComponent* timedSprite = (TimedSpriteComponent*)components->GetComponentData(explosion, C_TimedSprite);
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
                ColliderComponent* projectileCollider = (ColliderComponent*)components->GetComponentData(entity, C_Collider);
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
                    
                    if (g_Engine.entityManager.HasComponent(otherEntity, C_Enemy | C_Collider | C_Transform)) {
                        ColliderComponent* enemyCollider = (ColliderComponent*)components->GetComponentData(otherEntity, C_Collider);
                        TransformComponent* enemyTransform = (TransformComponent*)components->GetComponentData(otherEntity, C_Transform);
                        
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
                            EnemyComponent* enemy = (EnemyComponent*)components->GetComponentData(otherEntity, C_Enemy);
                            if (enemy) {
                                enemy->currHealth -= projectile->damage;
                            }
                            PlaySound::PlaySound(SOUND_HIT_NOISE);
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
                
                ADD_Transform(ccEntity, projectile->targetX, projectile->targetY, 0.0f, 1.0f);
                ADD_Crowdcontrol(ccEntity, projectile->targetEntity, projectile->targetX, projectile->targetY);
                ADD_LifeTime(ccEntity, 1.0f);
                ADD_TimedSprite(ccEntity, .8f, .2f, 1, 4);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[0] = ResourceManager::GetTexture(TEXTURE_GUST_1);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[1] = ResourceManager::GetTexture(TEXTURE_GUST_2);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[2] = ResourceManager::GetTexture(TEXTURE_GUST_3);
                g_Engine.componentArrays.TimedSprites[ccEntity].sprites[3] = ResourceManager::GetTexture(TEXTURE_GUST_4);

                g_mainGame.DeleteEntity(entity);
                PlaySound::PlaySound(SOUND_BLIP_HIGH);
            }

            if (projectile->type == PROJECTILE_LIGHTNING)
            {
                // create a chain entity
                // chain entity jumps from entity to entity drawing a purple rectangle
                // starts with tower, first entity, then go to the next
                // the chain entity also needs to not hit the same entity and destroy itself after x jumps, or if doesnt found enough entities to jump to.
                EntityID chain = g_mainGame.RegisterEntity();
                ADD_ChainLightning(chain, transform->x, transform->y, projectile->targetX, projectile->targetY, projectile->targetEntity, projectile->damage, 5);
                ADD_LifeTime(chain, .3f); // in case we forget to delete

                // deletes the projectile
                g_mainGame.DeleteEntity(entity);
                PlaySound::PlaySound(SOUND_SHOOT_LOW);
            }
        }
    
    
    
        // deals with the CC entities. This could be its own system, but it is not for the timebeing.
        if (g_Engine.entityManager.HasComponent(entity, C_Crowdcontrol))
        {
            CrowdcontrolComponent* cc = (CrowdcontrolComponent*)components->GetComponentData(entity, C_Crowdcontrol);
            if (cc && cc->target != INVALID_ENTITY) {
                // check if cc entity has lifetime component
                if (!g_Engine.entityManager.HasComponent(entity, C_LifeTime)) {
                    static bool ccLifetimeErrorPrinted = false;
                    if (!ccLifetimeErrorPrinted) {
                        printf("ERROR: cc entity missing lifetime component\n");
                        ccLifetimeErrorPrinted = true;
                    }
                }
                
                // freeze enemy at its position by removing movement components
                if (g_Engine.entityManager.HasComponent(cc->target, C_Transform)) {
                    TransformComponent* targetTransform = (TransformComponent*)components->GetComponentData(cc->target, C_Transform);
                    if (targetTransform) {
                        targetTransform->x = cc->targetX;
                        targetTransform->y = cc->targetY;
                    }
                }
            }
        }
    
        // deals with the CHAIN entities.  This could be its own system, but it is not for the timebeing.
        if (g_Engine.entityManager.HasComponent(entity, C_ChainLightning))
        {
            ChainLightningComponent* chain = (ChainLightningComponent*)components->GetComponentData(entity, C_ChainLightning);
            if (chain) {
                
                // if no jumps left, delete entity
                if (chain->jumps <= 0) {
                    g_mainGame.DeleteEntity(entity);
                    continue;
                }
                
                // increment frame delay counter
                chain->currFrameDelay++;
                
                // only process jump when frame delay is reached
                if (chain->currFrameDelay >= chain->frameDelay) {
                    // damage current target if valid and hasn't been damaged yet
                    if (chain->target != INVALID_ENTITY && !chain->hasDealtDamage) {
                        if (g_Engine.entityManager.HasComponent(chain->target, C_Enemy)) {
                            EnemyComponent* enemy = (EnemyComponent*)components->GetComponentData(chain->target, C_Enemy);
                            if (enemy) {
                                enemy->currHealth -= chain->damage;
                            }
                        }
                        
                        // add current target to hit list
                        for (int i = 0; i < 12; i++) {
                            if (chain->hits[i] == INVALID_ENTITY) {
                                chain->hits[i] = chain->target;
                                break;
                            }
                        }
                        
                        chain->hasDealtDamage = 1;
                    }
                    
                    // find next enemy to jump to
                    EntityID nextTarget = INVALID_ENTITY;
                    float closestDist = 150.0f; // max chain range
                    
                    for (EntityID enemy : entities) {
                        if (g_Engine.entityManager.HasComponent(enemy, C_Transform | C_Enemy)) {
                            // check if already hit
                            bool alreadyHit = false;
                            for (int i = 0; i < 12; i++) {
                                if (chain->hits[i] == enemy) {
                                    alreadyHit = true;
                                    break;
                                }
                            }
                            
                            if (!alreadyHit) {
                                TransformComponent* enemyTransform = (TransformComponent*)components->GetComponentData(enemy, C_Transform);
                                if (enemyTransform) {
                                    float dist = sqrt(pow(enemyTransform->x - chain->nextX, 2) + pow(enemyTransform->y - chain->nextY, 2));
                                    if (dist < closestDist) {
                                        closestDist = dist;
                                        nextTarget = enemy;
                                    }
                                }
                            }
                        }
                    }
                    
                    // update chain position and decrement jumps, or delete if no more targets
                    if (nextTarget != INVALID_ENTITY) {
                        TransformComponent* nextTransform = (TransformComponent*)components->GetComponentData(nextTarget, C_Transform);
                        if (nextTransform) {
                            chain->currX = chain->nextX;
                            chain->currY = chain->nextY;
                            chain->nextX = (int)nextTransform->x;
                            chain->nextY = (int)nextTransform->y;
                            chain->target = nextTarget;
                            chain->jumps--; // decrement jumps
                            chain->currFrameDelay = 0; // reset frame delay for next jump
                            chain->hasDealtDamage = 0; // reset damage flag for next target
                        }
                    } else {
                        g_mainGame.DeleteEntity(entity);
                    }
                }
            }
        }
    
    }
}
