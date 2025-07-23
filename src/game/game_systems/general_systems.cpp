#include "general_systems.h"
#include "engine.h"
#include "macro_utils.h"
#include "utils.h"
#include "main_game_scene.h"

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

void static CastJetAtTarget(int srcX, int srcY, int  destX, int destY)
{
    EntityID jet = g_mainGame.RegisterEntity();

    ADD_JetAnimation(jet, srcX, srcY, destX, destY);
    ADD_LifeTime(jet, .2);

}

#include "play_sound.h"
static void SpawnProjectile(EntityID tower, SceneBase *scene, PROJECTILE_TYPE type)
{
    TargetComponent *tc = GET_Target(tower);
    TransformComponent *target_transform = nullptr;
    EntityID target = tc->target; // store target
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
        PlaySound::PlaySound(SOUND_SHOOT_LOW1);

        
        if (enemy)
        {
            enemy->currHealth -= tower_damage->damage;
        }
        break;

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
        // Do some checks once: all entities that have projectile spawner are required to also have:
        // tranform, cooldown, tower and damage components
        if( !g_Engine.entityManager.HasComponent(tower ,C_Damage))
        {
            DO_ONCE(printf("Entity %d has projectile spawner but does not have damage!\n", tower));
            return;
        }
        

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

void CollisionSystem(SceneBase *scene)
{
}
