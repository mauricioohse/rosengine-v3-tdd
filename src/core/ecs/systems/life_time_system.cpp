#include "life_time_system.h"
#include "engine.h"

void life_time_system::Init()
{
}

void life_time_system::Update(float deltaTime, std::vector<EntityID> entities)
{

    for (EntityID e : entities)
    {
        if ( HasComponent<LifeTimeComponent>(e) )
        {
            LifeTimeComponent* lt = Get<LifeTimeComponent>(e);
            lt->remaininglifeTime -= deltaTime;
            
            if (lt->remaininglifeTime < 0 )
                g_Engine.entityManager.DestroyEntity(e);

        }
    }

}

void life_time_system::Destroy()
{
}
