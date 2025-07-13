#include "life_time_system.h"
#include "engine.h"

void life_time_system::Init()
{
}

void life_time_system::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays *components)
{

    for (EntityID e : entities)
    {
        if ( g_Engine.entityManager.HasComponent(e, COMPONENT_LIFETIME) )
        {
            LifeTimeComponent * lt = (LifeTimeComponent *)g_Engine.componentArrays.GetComponentData(e, COMPONENT_LIFETIME);
            
            lt->remaininglifeTime -= deltaTime;
            
            if (lt->remaininglifeTime < 0 )
                g_Engine.entityManager.DestroyEntity(e);

            

        }
    }

}

void life_time_system::Destroy()
{
}
