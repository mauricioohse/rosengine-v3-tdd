#include "systems.h"
#include "scene_manager.h"

// only looks into entities that have towers + range + projectile spawners that dont have TargetComponent yet
// adds TargetComponent to that entity
void TargetingSystem(SceneBase *scene);

// Spawns projectile based on tower components: target, CD, ProjectileSpawner, Damage
void ProjectileSpawningSystem(SceneBase *scene);

// reduce CD every frame
void AttackCDSystem(SceneBase *scene, float deltaTime);

// for collision + damage + damage_on_collision, deals damage on collision
// for collision + damage + area_damage, deals damage on area collision
void DamageSystem(SceneBase *scene);

void DeletionSystem(SceneBase *scene);
// deletes all entities with DeleteTagComponent

void CollisionSystem(SceneBase *scene);

void ExplodeOnXYSystem(SceneBase *scene);