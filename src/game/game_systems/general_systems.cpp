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

void TargetingSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT_2(scene, tower, Target, tar, Tower, tc)
    {
        // write logic here that uses tar and e data
        // For example:
        if (tar->target == 0) {
            tar->target= CheckEnemyInRange(tower);
            if (tar->target) printf("Tower %d found target tar->target %d!\n", tower, tar->target);
        }
    } END_FOR_EACH
}

void ProjectileSpawningSystem(SceneBase *scene)
{
}

void AttackCDSystem(SceneBase *scene, float deltaTime)
{
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
