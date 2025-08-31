#include "general_systems.h"
#include "engine.h"
#include "macro_utils.h"
#include "utils.h"
#include "main_game_scene.h"
#include "play_sound.h"
#include "math.h"



static EntityID CheckEnemyInRange(EntityID tower){

    SceneBase* scene = &g_mainGame;

    TowerComponent* tower_c      = Get<TowerComponent>(tower);
    TransformComponent* tower_tr  = Get<TransformComponent>(tower);

    // we create a fake collider based on the tower range
    ColliderComponent tower_cc;
    tower_cc.radius     = tower_c->range;
    tower_cc.isStatic   = 1;
    tower_cc.isTrigger  = 0;
    EntityID enemyInRange = INVALID_ENTITY;
    
    // check collision with enemies
    ForEachComponent<TransformComponent, EnemyComponent, ColliderComponent>(scene, [&](EntityID enemy, TransformComponent* enemy_tr, EnemyComponent* enemy_c, ColliderComponent* enemy_cc) {

            float penX, penY; //used to get the penetration of the collision, but we dont care in this case
            if(CheckCollision(tower_tr, &tower_cc, enemy_tr, enemy_cc, penX, penY))
            {
                // printf("collision detected! tower pos: (%.2f, %.2f) size: (%.2f, %.2f) enemy pos: (%.2f, %.2f) size: (%.2f, %.2f)\n", 
                //        tr->x, tr->y, cc_range.width, cc_range.height, 
                //        enemy_tr->x, enemy_tr->y, enemy_cc->width, enemy_cc->height);
                enemyInRange = enemy;
                return false; // enemy found, breaks loop
            }
            return true; // continue searching
    });

    return enemyInRange; // zero -> didnt find any valid enemy 

}

static bool IsEnemyInRange(EntityID enemy, int tower_x, int tower_y, int range)
{
    auto enemy_tr = Get<TransformComponent>(enemy);

    float dx = enemy_tr->x - tower_x;
    float dy = enemy_tr->y - tower_y;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance <= range) { // explosion radius hardcoded for now
        return true;
    }

    return false;
}

void ExplodeOnXYSystem(SceneBase *scene)
{
    ForEachComponent<TransformComponent, ExplodeOnXYComponent>(scene, [&](EntityID entity, TransformComponent* tr, ExplodeOnXYComponent* exp)
    {
        const int TOLERANCE = 10; // 10px tolerance each side
        if (abs(exp->x - tr->x) <= TOLERANCE && abs(exp->y - tr->y) <= TOLERANCE)
        {
            // reached target position, trigger explosion
            EntityID explosion = scene->RegisterEntity();
            TransformComponent::Add(explosion, tr->x, tr->y, 0.0f, (exp->range/75.0) );
            LifeTimeComponent::Add(explosion, 0.6f);
            PlaySound::PlaySound(SOUND_BOOM_LOW);


            auto timedSprite = TimedSpriteComponent::Add(explosion, 0, 0.2f, 0, 3);
            if (timedSprite) {
                timedSprite->sprites[0] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_1);
                timedSprite->sprites[1] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_2);
                timedSprite->sprites[2] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_3);
            }

            // deal damage to nearby enemies
            DamageComponent* dmg = Get<DamageComponent>(entity);
            if (dmg) {
                ForEachComponent<EnemyComponent, TransformComponent, ColliderComponent>(scene, [&](EntityID enemy, EnemyComponent* en, TransformComponent* enemy_tr, ColliderComponent* enemy_cc)
                {
                
                    if (IsEnemyInRange(enemy, exp->x, exp->y, exp->range))
                        en->currHealth -= dmg->damage;
                    return true; // continue processing
                });
            }
            scene->DeleteEntity(entity);
        }
    });
}

void DamageOnCollisionSystem(SceneBase * scene)
{
    ForEachComponent<DamageOnCollisionComponent, DamageComponent, ColliderComponent, TransformComponent>(scene, [&](EntityID damage_entity, auto* unused, auto* entity_dmg, auto* entity_col, auto* entity_tr)
    {
        ForEachComponent<EnemyComponent, TransformComponent, ColliderComponent>(scene, [&](EntityID enemy, auto* enemy_component, auto* enemy_tr, auto* enemy_col)
        {
            float penX, penY;
            if (CheckCollision(entity_tr, entity_col, enemy_tr, enemy_col, penX, penY))
            {

                enemy_component->currHealth -= entity_dmg->damage;

                // also checks for AddSlowOnCollision
                auto add_slow = Get<AddSlowOnCollisionComponent>(damage_entity);
                if (nullptr != add_slow)
                {
                    SlowComponent::Add(enemy, add_slow->intensity, add_slow->duration);
                }

                PlaySound::PlaySound(SOUND_HIT_NOISE);
                g_mainGame.DeleteEntity(damage_entity);
                return false;
            }
            return true; // continue searching
        });
    });
}

void new_CrowdcontrolSystem(SceneBase * scene)
{
    ForEachComponent<TransformComponent, CrowdControlComponent>(scene, [&](EntityID entity, TransformComponent* tc, CrowdControlComponent* CC)
    {
        TransformComponent * enemy_tr = Get<TransformComponent>(CC->target);
        // this locks the enemy_tr x and y to the data saved in the CC component
        if(enemy_tr)
        {
            enemy_tr->x = CC->targetX;
            enemy_tr->y = CC->targetY;
        }
    });
}

void static CastJetAtTarget(int srcX, int srcY, int  destX, int destY)
{
    EntityID jet = g_mainGame.RegisterEntity();

    JetAnimationComponent::Add(jet, srcX, srcY, destX, destY);
    LifeTimeComponent::Add(jet, .2);
    PlaySound::PlaySound(SOUND_SHOOT_LOW1);
}

static void CreateExplosionAt(SceneBase * scene, int x, int y, int range, int damage)
{
    EntityID bomb = scene->RegisterEntity();
    TransformComponent::Add(bomb, x, y, 0, 1);
    DamageComponent::Add(bomb, damage);
    ExplodeOnXYComponent::Add(bomb, x, y, range);
}

// TODO: think of a better way to separate the chainlightning into smaller components
void ChainLightningSystem(SceneBase * scene)
{
    ForEachComponent<ChainLightningComponent>(scene, [&](EntityID entity, ChainLightningComponent* CL)
    {
        // if no jumps left, delete entity
        if (CL->jumps <= 0)
        {
            g_mainGame.DeleteEntity(entity);
            return true; // skips to next loop iteration
        }

        // increment frame delay counter
        CL->currFrameDelay++;

        // only process jump when frame delay is reached
        if (CL->currFrameDelay >= CL->frameDelay)
        {
            // damage current target if valid and hasn't been damaged yet
            if (CL->target != INVALID_ENTITY && !CL->hasDealtDamage)
            {
                if (HasComponent<EnemyComponent>(CL->target))
                {
                    EnemyComponent* enemy = Get<EnemyComponent>(CL->target);
                    if (nullptr != enemy)
                    {
                        if (CL->explodes)
                        {
                            auto target_transform = Get<TransformComponent>(CL->target);
                            // creates a mini explosion on the target
                            CreateExplosionAt(scene, target_transform->x, target_transform->y, 30, CL->damage);
                        }
                        else // individual damage only
                        {
                            enemy->currHealth -= CL->damage;
                        }
                    }
                }

                // add current target to hit list
                for (int i = 0; i < 12; i++)
                {
                    if (CL->hits[i] == INVALID_ENTITY)
                    {
                        CL->hits[i] = CL->target;
                        return false;
                    }
                }

                CL->hasDealtDamage = 1;
            }

            // find next enemy to jump to
            EntityID nextTarget = INVALID_ENTITY;
            float closestDist = 150.0f; // max chain range

            ForEachComponent<TransformComponent, EnemyComponent>(scene, [&](EntityID enemy, TransformComponent* enemy_tr, EnemyComponent* enemy_c)
            {
                // check if already hit
                bool alreadyHit = false;
                for (int i = 0; i < 12; i++)
                {
                    if (CL->hits[i] == enemy)
                    {
                        alreadyHit = true;
                        return false;
                    }
                }

                if (!alreadyHit) // note: this conditional seems redundant
                {
                    // TODO: this should probably use the already existing target system instead of doing it again here. or create a component "TargetAndRemember"
                    float dist = sqrt(pow(enemy_tr->x - CL->nextX, 2) + pow(enemy_tr->y - CL->nextY, 2));
                    if (dist < closestDist)
                    {
                        closestDist = dist;
                        nextTarget = enemy;
                    }
                }
            });

            // update chain position and decrement jumps, or delete if no more targets
            if (nextTarget != INVALID_ENTITY)
            {
                TransformComponent *nextTransform = Get<TransformComponent>(nextTarget);
                if (nextTransform)
                {
                    CL->currX = CL->nextX;
                    CL->currY = CL->nextY;
                    CL->nextX = (int)nextTransform->x;
                    CL->nextY = (int)nextTransform->y;
                    CL->target = nextTarget;
                    CL->jumps--;            // decrement jumps
                    CL->currFrameDelay = 0; // reset frame delay for next jump
                    CL->hasDealtDamage = 0; // reset damage flag for next target
                }
            }
        }
        return true;
    });
}

static void CreateCCGust(SceneBase* scene, EntityID target)
{
    // creates a CC entity with gust sprite. delete the projectile, leave the CC entity to deal with the enemy.
    EntityID gust_entity = scene->RegisterEntity();
    auto target_transform = Get<TransformComponent>(target);

    TransformComponent::Add(gust_entity, target_transform->x, target_transform->y, 0.0f, 1.0f);
    CrowdControlComponent::Add(gust_entity, target, target_transform->x, target_transform->y);
    LifeTimeComponent::Add(gust_entity, 1.0f);
    TimedSpriteComponent::Add(gust_entity, .8f, .2f, 1, 4);

    Get<TimedSpriteComponent>(gust_entity)->sprites[0] = ResourceManager::GetTexture(TEXTURE_GUST_1);
    Get<TimedSpriteComponent>(gust_entity)->sprites[1] = ResourceManager::GetTexture(TEXTURE_GUST_2);
    Get<TimedSpriteComponent>(gust_entity)->sprites[2] = ResourceManager::GetTexture(TEXTURE_GUST_3);
    Get<TimedSpriteComponent>(gust_entity)->sprites[3] = ResourceManager::GetTexture(TEXTURE_GUST_4);

    PlaySound::PlaySound(SOUND_BLIP_HIGH);
}


static EntityID GetTarget(TransformComponent* tr, TowerComponent * tw )
{
    SceneBase * scene = &g_mainGame;
    
    EntityID bestTarget = 0;
    int maxPathProgress = -1;
    
    // iterate through all enemies
    ForEachComponent<TransformComponent, EnemyComponent>(scene, [&](EntityID enemy, TransformComponent* enemy_tr, EnemyComponent* en)
    {
        // check if enemy is in range
        float dx = enemy_tr->x - tr->x;
        float dy = enemy_tr->y - tr->y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance <= tw->range) {
            // enemy is in range, check if it's more advanced than current best
            if (en->currPathIdx > maxPathProgress) {
                maxPathProgress = en->currPathIdx;
                bestTarget = enemy;
            }
        }
    });
    
    return bestTarget;
}


// TODO: refactor this into smaller functions that each spawn specific projectile + have a logic for the targeting
static void SpawnProjectile(EntityID tower, SceneBase *scene, PROJECTILE_TYPE type)
{
    TransformComponent* tower_transform = Get<TransformComponent>(tower);
    TowerComponent* tower_component = Get<TowerComponent>(tower);
    
    if (!tower_transform || !tower_component) return;
    
    EntityID target = GetTarget(tower_transform, tower_component);
    TransformComponent *target_transform = nullptr;
    
    if (target != 0 && g_Engine.entityManager.IsEntityValid(target))
    {
        target_transform = Get<TransformComponent>(target);
    }
    else
        return; // no target, do nothing

    // if (!ps) {DO_ONCE(printf("something wrong happened here!\n"); return;);} // how the hell we came here!?

    DamageComponent* tower_damage = Get<DamageComponent>(tower);
    EnemyComponent*enemy = Get<EnemyComponent>(target);

    EntityID projectile = scene->RegisterEntity();

    switch (type)
    {
    case PROJECTILE_BOMB:
        printf("creating projectile!\n");

        TransformComponent::Add(projectile, tower_transform->x, tower_transform->y, 0, 1);
        MoveToXYComponent::Add(projectile, target_transform->x, target_transform->y, 200);
        SpriteComponent::Add(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE));
        DamageComponent::Add(projectile, tower_damage->damage);
        ExplodeOnXYComponent::Add(projectile, target_transform->x, target_transform->y, tower_component->AOEradius);
        break; 
    case PROJECTILE_JET:
        CastJetAtTarget(tower_transform->x, tower_transform->y, target_transform->x, target_transform->y);
        if (enemy)
        {
            enemy->currHealth -= tower_damage->damage;
        }

        break;

    case PROJECTILE_PELLET:
    {
        TransformComponent::Add(projectile, tower_transform->x, tower_transform->y, 0, 1);
        SpriteComponent::Add(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE_BROWN));
        ColliderComponent::Add(projectile,27,0,0);
        MoveToXYComponent::Add(projectile, target_transform->x, target_transform->y, 400);
        DamageComponent::Add(projectile, tower_damage->damage);
        DamageOnCollisionComponent::Add(projectile);
    }
    break;

    case  PROJECTILE_GUST:
        // creates a CC entity with gust sprite. delete the projectile, leave the CC entity to deal with the enemy.
        printf("gust projectile created!\n");
        CreateCCGust(scene, target);
        
        break;

    case PROJECTILE_LIGHTNING:
        ChainLightningComponent::Add(projectile, tower_transform->x, tower_transform->y,
                           target_transform->x, target_transform->y, target, tower_damage->damage, 3, 0);
        LifeTimeComponent::Add(projectile, .3f); // in case we forget to delete
        PlaySound::PlaySound(SOUND_SHOOT_LOW);
        break;

    case PROJECTILE_JET_BOMB:
        {
            // jet bomb is actually two entities: one jet with 0 damage, and a exploding bomb
            CastJetAtTarget(tower_transform->x, tower_transform->y, target_transform->x, target_transform->y); // this it just the animation

            CreateExplosionAt(scene, target_transform->x, target_transform->y, 50, tower_damage->damage);
            scene->DeleteEntity(projectile);
        
        }
    break;
    case PROJECTILE_AREA_GUST:
        // area gust queries for all enemies in tower range, then spawn a CC entity for each enemy that will despawn shortly
        // TODO: in the future we should not do a range check here, I think there should be a "AreaRange" component or something like that
        ForEachComponent<EnemyComponent, TransformComponent>(scene, [&](EntityID enemy, EnemyComponent* enemy_en, TransformComponent* enemy_tr)
        {
            auto tower_c = Get<TowerComponent>(tower);
            if (IsEnemyInRange(enemy, tower_transform->x, tower_transform->y, tower_c->range))
            {
                printf("area gust single CC created!\n");
                CreateCCGust(scene, enemy);
            }
        });

    break;

    case PROJECTILE_EXPLODING_LIGHTNING:
        ChainLightningComponent::Add(projectile, tower_transform->x, tower_transform->y,
                           target_transform->x, target_transform->y, target, tower_damage->damage, 5, 1);
        PlaySound::PlaySound(SOUND_SHOOT_LOW);

        break;

    case PROJECTILE_ICE_SHARD:
        // similar to pellet, but adds slow on collision
        AddSlowOnCollisionComponent::Add(projectile, .5, .2); // Note: hardcoded
        TransformComponent::Add(projectile, tower_transform->x, tower_transform->y, 0, 1);
        SpriteComponent::Add(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE_ICE_SHARD));
        ColliderComponent::Add(projectile, 54,0,0);
        MoveToXYComponent::Add(projectile, target_transform->x, target_transform->y, 500);
        DamageComponent::Add(projectile, tower_damage->damage);
        DamageOnCollisionComponent::Add(projectile);
        PlaySound::PlaySound(SOUND_SHOOT_LOW);

    break;

    default:
        DO_ONCE(printf("forgot to set the projectileSpawner type!\n"););
        break;
    }

}

void ProjectileSpawningSystem(SceneBase *scene)
{
    ForEachComponent<ProjectileSpawnerComponent, TowerComponent, CooldownComponent, TransformComponent>(scene, [&](EntityID tower, ProjectileSpawnerComponent* spawner, TowerComponent* tc, CooldownComponent* cd, TransformComponent* tr)
    {
        if(cd->remainingCD<0)
        {
            // SPAWNING TIME!!!1!!!
            SpawnProjectile(tower, scene, spawner->type);
            cd->remainingCD = cd->CD;
        }
        

    });
}

void AttackCDSystem(SceneBase *scene, float deltaTime)
{
    ForEachComponent<CooldownComponent>(scene, [&](EntityID entity, CooldownComponent* cd)
    {
        cd->remainingCD-=deltaTime;
    });
}


void DamageSystem(SceneBase *scene)
{
}

void DeletionSystem(SceneBase *scene)
{
}


