#include "enemy_spawner.h"
#include "engine.h"
#include "main_game_scene.h"

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
    Get<ColliderComponent>(enemy)->radius = Get<SpriteComponent>(enemy)->height / 2; // copy the collider size from the sprite size

    return enemy;

}

