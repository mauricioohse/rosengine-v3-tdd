#include "general_systems.h"
#include "engine.h"
#include "macro_utils.h"
#include "utils.h"

static EntityID CheckEnemyInRange(EntityID tower, TowerComponent * tc, TransformComponent * tr,  std::vector<EntityID> entities){

    // check collision with enemies
    for (EntityID enemy: entities)
    {
        if(g_Engine.entityManager.HasComponent(enemy, C_Transform | C_Enemy |  C_COLLIDER))
        {
            // TODO: change to circular collision check instead of AABB
            TransformComponent * enemy_tr = (TransformComponent *)g_Engine.componentArrays.GetComponentData(enemy, C_Transform);
            ColliderComponent * enemy_cc = (ColliderComponent *)g_Engine.componentArrays.GetComponentData(enemy, C_COLLIDER);

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
            

        }
        
    }

    return 0xFFFFFFFFu;

}

void TargetingSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT(scene, e, Target, tar)
    {
        // write logic here that uses tar and e data
        // For example:
        if (tar->target == 0) {
            printf("Entity %u needs a new target!\n", e);
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
