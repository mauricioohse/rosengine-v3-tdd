#include "general_systems.h"
#include "engine.h"
#include "macro_utils.h"

void TargetingSystem(SceneBase *scene)
{
    FOR_EACH_COMPONENT(scene, e, TargetComponent, COMPONENT_TARGET, tar)
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
