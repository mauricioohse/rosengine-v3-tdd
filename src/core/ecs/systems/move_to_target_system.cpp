#include "move_to_target_system.h"
#include "engine.h"
#include "math.h"
#include "../component_manager.h"

void move_to_target_system::Init()
{
}

void move_to_target_system::Update(float deltaTime, std::vector<EntityID> entities)
{
    for (EntityID e : entities)
    {
        if (HasComponents<TransformComponent, MoveToXYComponent>(e))
        {
            TransformComponent* transform = Get<TransformComponent>(e);
            MoveToXYComponent* moveTarget = Get<MoveToXYComponent>(e);


            // calculate direction to target
            float dx = moveTarget->targetX - transform->x;
            float dy = moveTarget->targetY - transform->y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 1.0f)
            {
                // normalize and apply speed
                dx /= distance;
                dy /= distance;

                transform->x += dx * moveTarget->speed * deltaTime;
                transform->y += dy * moveTarget->speed * deltaTime;
            }
        }
    }

}

void move_to_target_system::Destroy()
{
}
