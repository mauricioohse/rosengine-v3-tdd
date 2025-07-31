#include "general_systems.h"
#include "engine.h"
#include "macro_utils.h"
#include "utils.h"
#include "main_game_scene.h"
#include "play_sound.h"


static EntityID CheckEnemyInRange(EntityID tower){

    SceneBase * scene = &g_mainGame;

    TowerComponent* tc = (TowerComponent*)g_Engine.componentArrays.GetComponentData(tower, C_Tower); 
    TransformComponent* tr = (TransformComponent*)g_Engine.componentArrays.GetComponentData(tower, C_Transform); 

    // check collision with enemies
    FOR_EACH_COMPONENT_3(scene, enemy, 
                        Transform, enemy_tr,
                        Enemy, en,
                        Collider, enemy_cc)
    {

            // we create a fake collider based on the tower range
            ColliderComponent cc_range;
            cc_range.height= tc->range;
            cc_range.width= tc->range;
            cc_range.isStatic= 1;
            cc_range.isTrigger=0;

            float penX,penY; //used to get the penetration of the collision, but we dont care in this case
            if(CheckCollisionCentered(tr,&cc_range, enemy_tr, enemy_cc, penX, penY))
            {
                // printf("collision detected! tower pos: (%.2f, %.2f) size: (%.2f, %.2f) enemy pos: (%.2f, %.2f) size: (%.2f, %.2f)\n", 
                //        tr->x, tr->y, cc_range.width, cc_range.height, 
                //        enemy_tr->x, enemy_tr->y, enemy_cc->width, enemy_cc->height);

                return enemy;
            }
            

                
    } END_FOR_EACH

    return 0; // zero -> didnt find any valid enemy 

}

// TODO: this targeting system sucks and needs to be better thought out. sometimes towers shoots entities outside their range if the target is aquired while CD > 0
void TargetingSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT_2(scene, tower, Target, tar, Tower, tc)
    {

        if (tar->target == 0 && tc->currCD<=0) { // TODO: create a INVALID_ENTITY constant instead of magic 0
            tar->target= CheckEnemyInRange(tower);
            if (tar->target) printf("Tower %d found target tar->target %d!\n", tower, tar->target);
        }
        else
        {
            if (!g_Engine.entityManager.IsEntityValid(tar->target))
                tar->target = 0;
        }
        
    } END_FOR_EACH
}

#include "math.h"
void ExplodeOnXYSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT_2(scene, entity,
                         Transform, tr,
                         ExplodeOnXY, exp)
    {
        const int TOLERANCE = 10; // 10px tolerance each side
        if (abs(exp->x - tr->x) <= TOLERANCE && abs(exp->y - tr->y) <= TOLERANCE)
        {
            // reached target position, trigger explosion
            EntityID explosion = scene->RegisterEntity();
            ADD_Transform(explosion, tr->x, tr->y, 0.0f, 1.0f);
            ADD_TimedSprite(explosion, 0, 0.2f, 0, 3);
            ADD_LifeTime(explosion, 0.6f);
            PlaySound::PlaySound(SOUND_BOOM_LOW);
            
            TimedSpriteComponent* timedSprite = GET_TimedSprite(explosion);
            if (timedSprite) {
                timedSprite->sprites[0] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_1);
                timedSprite->sprites[1] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_2);
                timedSprite->sprites[2] = ResourceManager::GetTexture(TEXTURE_EXPLOSION_3);
            }
            
            // deal damage to nearby enemies
            DamageComponent* dmg = GET_Damage(entity);
            if (dmg) {
                FOR_EACH_COMPONENT_3(scene, enemy,
                                   Transform, enemy_tr,
                                   Enemy, en,
                                   Collider, enemy_cc)
                {
                    float dx = enemy_tr->x - tr->x;
                    float dy = enemy_tr->y - tr->y;
                    float distance = sqrt(dx * dx + dy * dy);
                    
                    if (distance <= 100) { // explosion radius hardcoded for now
                        en->currHealth -= dmg->damage;
                    }

                } END_FOR_EACH

            }
            
            scene->DeleteEntity(entity);
        }
    }
    END_FOR_EACH
}

void DamageOnCollisionSystem(SceneBase * scene)
{
    FOR_EACH_COMPONENT_4(scene, damage_entity,
                         DamageOnCollision, unused,
                         Damage, entity_dmg,
                         Collider, entity_col,
                         Transform, entity_tr)
    {
        // look for enemies that collide with the entity
        FOR_EACH_COMPONENT_3(scene, enemy,
                             Enemy, enemy_component,
                             Transform,
                             enemy_tr,
                             Collider, enemy_col)
        {
            float penX, penY;
            if (CheckCollisionCentered(entity_tr, entity_col, enemy_tr, enemy_col, penX, penY))
            {

                enemy_component->currHealth -= entity_dmg->damage;

                PlaySound::PlaySound(SOUND_HIT_NOISE);
                g_mainGame.DeleteEntity(damage_entity);
                break;
            }
        } END_FOR_EACH
    } END_FOR_EACH

}

void new_CrowdcontrolSystem(SceneBase * scene)
{

    FOR_EACH_COMPONENT_2(scene, entity,
                         Transform, tc,
                         Crowdcontrol, CC)
    {
        TransformComponent * enemy_tr = GET_Transform(CC->target);
        // this locks the enemy_tr x and y to the data saved in the CC component
        if(enemy_tr)
        {
            enemy_tr->x = CC->targetX;
            enemy_tr->y = CC->targetY;
        }
    } END_FOR_EACH
}

void static CastJetAtTarget(int srcX, int srcY, int  destX, int destY)
{
    EntityID jet = g_mainGame.RegisterEntity();

    ADD_JetAnimation(jet, srcX, srcY, destX, destY);
    ADD_LifeTime(jet, .2);
    PlaySound::PlaySound(SOUND_SHOOT_LOW1);


}

// TODO: think of a better way to separate the chainlightning into smaller components
void ChainLightningSystem(SceneBase * scene)
{
    FOR_EACH_COMPONENT(scene, entity,
                       ChainLightning, CL)
    {
        // if no jumps left, delete entity
        if (CL->jumps <= 0)
        {
            g_mainGame.DeleteEntity(entity);
            continue;
        }

        // increment frame delay counter
        CL->currFrameDelay++;

        // only process jump when frame delay is reached
        if (CL->currFrameDelay >= CL->frameDelay)
        {
            // damage current target if valid and hasn't been damaged yet
            if (CL->target != INVALID_ENTITY && !CL->hasDealtDamage)
            {
                if (g_Engine.entityManager.HasComponent(CL->target, C_Enemy))
                {
                    EnemyComponent *enemy = GET_Enemy(CL->target);
                    if (enemy)
                    {
                        enemy->currHealth -= CL->damage;
                    }
                }

                // add current target to hit list
                for (int i = 0; i < 12; i++)
                {
                    if (CL->hits[i] == INVALID_ENTITY)
                    {
                        CL->hits[i] = CL->target;
                        break;
                    }
                }

                CL->hasDealtDamage = 1;
            }

            // find next enemy to jump to
            EntityID nextTarget = INVALID_ENTITY;
            float closestDist = 150.0f; // max chain range

            FOR_EACH_COMPONENT_2(scene, enemy,
                                Transform, enemy_tr,
                                Enemy, enemy_c)
            {
                // check if already hit
                bool alreadyHit = false;
                for (int i = 0; i < 12; i++)
                {
                    if (CL->hits[i] == enemy)
                    {
                        alreadyHit = true;
                        break;
                    }
                }

                if (!alreadyHit)
                {
                    // NOTE: this should probably use the already existing target system instead of doing it again here. or create a component "TargetAndRemember"
                    float dist = sqrt(pow(enemy_tr->x - CL->nextX, 2) + pow(enemy_tr->y - CL->nextY, 2));
                    if (dist < closestDist)
                    {
                        closestDist = dist;
                        nextTarget = enemy;
                    }
                }
            }
            END_FOR_EACH

            // update chain position and decrement jumps, or delete if no more targets
            if (nextTarget != INVALID_ENTITY)
            {
                TransformComponent *nextTransform = GET_Transform(nextTarget);
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
    } END_FOR_EACH
}

// TODO: refactor this into smaller functions that each spawn specific projectile + have a logic for the targeting
static void SpawnProjectile(EntityID tower, SceneBase *scene, PROJECTILE_TYPE type)
{
    TargetComponent *tc = GET_Target(tower);
    TransformComponent *target_transform = nullptr;
    EntityID target = tc ? tc->target : INVALID_ENTITY; // store target
    if (target!=0 && g_Engine.entityManager.IsEntityValid(target))
    {
        target_transform = GET_Transform(tc->target);
        tc->target = 0; // resets target
    }
    else
    return ; // no target, do nothing

    // if (!ps) {DO_ONCE(printf("something wrong happened here!\n"); return;);} // how the hell we came here!?

    TransformComponent* tower_transform = GET_Transform(tower);
    DamageComponent * tower_damage = GET_Damage(tower);
    EnemyComponent *enemy = GET_Enemy(target);

    EntityID projectile = scene->RegisterEntity();

    switch (type)
    {
    case PROJECTILE_BOMB:
        printf("creating projectile!\n");

        ADD_Transform(projectile, tower_transform->x, tower_transform->y, 0, 1);
        ADD_MoveToXY(projectile, target_transform->x, target_transform->y, 200);
        ADD_Sprite(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE));
        ADD_Damage(projectile, 50);
        ADD_ExplodeOnXY(projectile, target_transform->x, target_transform->y);
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
        ADD_Transform(projectile, tower_transform->x, tower_transform->y, 0, 1);
        ADD_Sprite(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE_BROWN));
        ADD_COLLIDER(projectile, 27,27,0,0);
        ADD_MoveToXY(projectile, target_transform->x, target_transform->y, 400);
        ADD_Damage(projectile, 25);
        ADD_DamageOnCollision(projectile);
    }
    break;

    case  PROJECTILE_GUST:
        // creates a CC entity with gust sprite. delete the projectile, leave the CC entity to deal with the enemy.
        printf("gust projectile created!\n");

        ADD_Transform(projectile, target_transform->x, target_transform->y, 0.0f, 1.0f);
        ADD_Crowdcontrol(projectile, target, target_transform->x, target_transform->y);
        ADD_LifeTime(projectile, 1.0f);
        ADD_TimedSprite(projectile, .8f, .2f, 1, 4);
        g_Engine.componentArrays.TimedSprites[projectile].sprites[0] = ResourceManager::GetTexture(TEXTURE_GUST_1);
        g_Engine.componentArrays.TimedSprites[projectile].sprites[1] = ResourceManager::GetTexture(TEXTURE_GUST_2);
        g_Engine.componentArrays.TimedSprites[projectile].sprites[2] = ResourceManager::GetTexture(TEXTURE_GUST_3);
        g_Engine.componentArrays.TimedSprites[projectile].sprites[3] = ResourceManager::GetTexture(TEXTURE_GUST_4);
        PlaySound::PlaySound(SOUND_BLIP_HIGH);
        
        break;

    case PROJECTILE_LIGHTNING:
        ADD_ChainLightning(projectile, tower_transform->x, tower_transform->y, 
                           target_transform->x, target_transform->y, target, tower_damage->damage, 5);
        ADD_LifeTime(projectile, .3f); // in case we forget to delete
        PlaySound::PlaySound(SOUND_SHOOT_LOW);

        break;

    case PROJECTILE_JET_BOMB:
        {
            // jet bomb is actually two entities: one jet with 0 damage, and a exploding bomb
            CastJetAtTarget(tower_transform->x, tower_transform->y, target_transform->x, target_transform->y); // this it just the animation

            EntityID bomb = projectile;
            ADD_Transform(bomb, target_transform->x, target_transform->y, 0, 1);
            ADD_Damage(bomb, tower_damage->damage);
            ADD_ExplodeOnXY(bomb, target_transform->x, target_transform->y); // note: explosion radius is hardcoded for now
        }
    default:
        DO_ONCE(printf("forgot to set the projectileSpawner type!\n"););
        break;
    }

}

void ProjectileSpawningSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT_4(scene, tower,
                         ProjectileSpawner, spawner,
                         Tower, tc,
                         Cooldown, cd,
                         Transform, tr )
    {

        if(cd->remainingCD<0)
        {
            // it is time to spawn
            SpawnProjectile(tower, scene, spawner->type);
            cd->remainingCD = cd->CD;
        }
        

    } END_FOR_EACH

}

void AttackCDSystem(SceneBase *scene, float deltaTime)
{
    FOR_EACH_COMPONENT(scene, entity,
                       Cooldown, cd)
    {
        cd->remainingCD-=deltaTime;
    }
    END_FOR_EACH
}


void DamageSystem(SceneBase *scene)
{
}

void DeletionSystem(SceneBase *scene)
{
}


