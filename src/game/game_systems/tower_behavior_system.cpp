#include "tower_behavior_system.h"
#include "engine.h"
#include "window.h"
#include "grid.h"
#include "utils.h"
#include "main_game_scene.h"

void tower_behavior_system::Init()
{
}


static void CreateProjectile(EntityID tower, TowerComponent * tc, EntityID enemy)
{
    TransformComponent* enemy_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(enemy, C_Transform);
    
    EntityID projectile = g_mainGame.RegisterEntity();
    
    TransformComponent* tower_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(tower, C_Transform);
    
    ADD_Transform(projectile, tower_transform->x, tower_transform->y, 0.0f, 1.0f);
    
    ADD_LifeTime(projectile, 2.0f); // by default projectiles expire in two seconds. mostly to ensure no stray projectile!

    // add projectile component with enemy position as target
    switch (tc->type)
    {
        default:
        printf("Tower has no expected projectile type selected! Defaulting to fire tower. \n");
        // no break; on purpose

        case TOWER_FIRE:
        ADD_Projectile(projectile, PROJECTILE_BOMB,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  50, 1, 100);
        ADD_Sprite(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE));
        ADD_MoveToXY(projectile, enemy_transform->x, enemy_transform->y, 200);

        break;
        case TOWER_WATER:
        ADD_Projectile(projectile, PROJECTILE_JET,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  200, 0, 0);
        ADD_LifeTime(projectile, 1.0f);
        break; 

        case TOWER_EARTH:
        ADD_Projectile(projectile, PROJECTILE_PELLET,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  25, 0, 0);
        ADD_Sprite(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE_BROWN));
        ADD_COLLIDER(projectile, 27,27,0,0);
        ADD_MoveToXY(projectile, enemy_transform->x, enemy_transform->y, 400);
        break;

        case TOWER_AIR:
        ADD_Projectile(projectile, PROJECTILE_GUST,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  0, 0, 0);
        break;

        case TOWER_ELECTRIC:
        ADD_Projectile(projectile, PROJECTILE_LIGHTNING,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  100, 0, 0);
        break;

        case TOWER_FIREWATER:
        ADD_Projectile(projectile, PROJECTILE_BOMB,enemy, (int)enemy_transform->x, (int)enemy_transform->y,  100, 1, 100);
        ADD_MoveToXY(projectile, enemy_transform->x, enemy_transform->y, 2500);

        EntityID jet = g_mainGame.RegisterEntity();
        ADD_Transform(jet, tower_transform->x, tower_transform->y, 0.0f, 1.0f);
        ADD_Projectile(jet, PROJECTILE_JET,enemy, (int)enemy_transform->x, (int)enemy_transform->y, 50, 0, 0);
        ADD_LifeTime(jet, 1.0f);
        break;

    }




    printf("projectile created: tower at (%.2f, %.2f) targeting enemy at (%d, %d)\n", 
           tower_transform->x, tower_transform->y, (int)enemy_transform->x, (int)enemy_transform->y);
}

static void HandleShooting(EntityID tower, TowerComponent * tc, EntityID enemy)
{

    if (tc->currCD<0)
    {
        tc->currCD = tc->attackCD; // resets the CD count
        CreateProjectile(tower, tc, enemy);
    }
}

static EntityID CheckEnemyInRange(EntityID tower, TowerComponent * tc, TransformComponent * tr,  std::vector<EntityID> entities){

    // check collision with enemies
    for (EntityID enemy: entities)
    {
        if(g_Engine.entityManager.HasComponent(enemy, C_Transform | C_Enemy |  C_Collider))
        {
            // TODO: change to circular collision check instead of AABB
            TransformComponent * enemy_tr = (TransformComponent *)g_Engine.componentArrays.GetComponentData(enemy, C_Transform);
            ColliderComponent * enemy_cc = (ColliderComponent *)g_Engine.componentArrays.GetComponentData(enemy, C_Collider);

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


void tower_behavior_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    for (EntityID e : entities)
    {
        if (g_Engine.entityManager.HasComponent(e, C_Transform | C_Tower /*| C_Collider*/ ) )
        {

            TowerComponent *tc = (TowerComponent *)g_Engine.componentArrays.GetComponentData(e, C_Tower);
            TransformComponent *tr = (TransformComponent *)g_Engine.componentArrays.GetComponentData(e, C_Transform);
            // ColliderComponent * cc = (ColliderComponent *)g_Engine.componentArrays.GetComponentData(e, C_Collider);

            // check if there is an enemy in range
            EntityID enemy = CheckEnemyInRange(e, tc, tr, entities);
            
            if (enemy != 0xFFFFFFFFu)
                HandleShooting(e, tc, enemy);

            // update attack CD
            tc->currCD-=deltaTime;
        }
    }

}

void tower_behavior_system::Destroy()
{
}


