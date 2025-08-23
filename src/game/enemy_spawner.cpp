#include "enemy_spawner.h"
#include "engine.h"
#include "main_game_scene.h"
#include "components.h"

// file to have functions for organizing enemy creation

EntityID EnemySpawner::SpawnEnemyAt(SceneBase* scene, int x, int y, ENEMY_TYPE type, bool debug)
{
    EntityID enemy = scene->RegisterEntity();
    EnemyArchetype enemy_archetype = g_enemy_archetypes[type];

    ColliderComponent::Add(enemy, 96, 1, 0);
    TransformComponent::Add(enemy,
                            (float)x, 
                            (float)y,
                            0.0F,
                            enemy_archetype.size_multiplier);
    EnemyComponent::Add(enemy, enemy_archetype.base_health, enemy_archetype.base_speed);
    SpriteComponent::Add(enemy, ResourceManager::GetTexture(enemy_archetype.texture));
    
    // Safety checks before accessing components
    ColliderComponent* collider = Get<ColliderComponent>(enemy);
    SpriteComponent* sprite = Get<SpriteComponent>(enemy);
    
    if (collider && sprite) {
        collider->radius = sprite->height / 2; // copy the collider size from the sprite size
        printf("Enemy %d spawned successfully with radius %f\n", enemy, collider->radius);
    } else {
        printf("ERROR: Failed to get components for enemy %d - collider: %p, sprite: %p\n", 
               enemy, collider, sprite);
    }

    return enemy;

}

