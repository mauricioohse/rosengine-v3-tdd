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
    // get enemy position for targeting
    TransformComponent* enemy_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(enemy, COMPONENT_TRANSFORM);
    
    // create new projectile entity
    EntityID projectile = g_mainGame.RegisterEntity();
    
    // get tower position for projectile spawn
    TransformComponent* tower_transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(tower, COMPONENT_TRANSFORM);
    
    // add transform component at tower position
    ADD_TRANSFORM(projectile, tower_transform->x, tower_transform->y, 0.0f, 1.0f);
    
    // add sprite component with basic projectile texture
    ADD_SPRITE(projectile, ResourceManager::GetTexture(TEXTURE_BASIC_PROJECTILE));
    
    // add projectile component with enemy position as target
    ADD_PROJECTILE(projectile, 3.0f, (int)enemy_transform->x, (int)enemy_transform->y, 200, 1, 100);

    ADD_MOVETOXY(projectile, enemy_transform->x, enemy_transform->y, 200);

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

static void HandleEnemyInRange(EntityID tower, TowerComponent * tc, TransformComponent * tr,  std::vector<EntityID> entities){

    // check collision with enemies
    for (EntityID enemy: entities)
    {
        if(g_Engine.entityManager.HasComponent(enemy, COMPONENT_TRANSFORM | COMPONENT_ENEMY |  COMPONENT_COLLIDER))
        {
            // TODO: change to circular collision check instead of AABB
            TransformComponent * enemy_tr = (TransformComponent *)g_Engine.componentArrays.GetComponentData(enemy, COMPONENT_TRANSFORM);
            ColliderComponent * enemy_cc = (ColliderComponent *)g_Engine.componentArrays.GetComponentData(enemy, COMPONENT_COLLIDER);

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

                HandleShooting(tower, tc, enemy);
            }
            

        }
        
    }

}

static void UpdateAttackCD(TowerComponent *tc,float deltaTime)
{
    tc->currCD-=deltaTime;
}

void tower_behavior_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{
    for (EntityID e : entities)
    {
        if (g_Engine.entityManager.HasComponent(e, COMPONENT_TRANSFORM | COMPONENT_TOWER | COMPONENT_COLLIDER))
        {

            TowerComponent *tc = (TowerComponent *)g_Engine.componentArrays.GetComponentData(e, COMPONENT_TOWER);
            TransformComponent *tr = (TransformComponent *)g_Engine.componentArrays.GetComponentData(e, COMPONENT_TRANSFORM);
            ColliderComponent * cc = (ColliderComponent *)g_Engine.componentArrays.GetComponentData(e, COMPONENT_COLLIDER);

            switch (tc->type)
            {
            case 1: // fire
                DrawRangeAroundTower(tr->x, tr->y, tc->range);
                break;
            default:
                // do nothing
                break;
            }

            HandleEnemyInRange(e, tc, tr, entities);

            UpdateAttackCD(tc, deltaTime);
        }
    }

}

void tower_behavior_system::Destroy()
{
}

static void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void tower_behavior_system::DrawRangeAroundTower(float x, float y, int radius){
    Point p = Grid::GetNearestGridPointCenter(x,y);


    SDL_Renderer * renderer = g_Engine.window->renderer;    

    SDL_SetRenderDrawColor(renderer, 255, 0 , 125, 125);
    DrawCircle(renderer, p.x, p.y, radius);
}