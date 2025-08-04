#include "enemy_spawner.h"
#include "engine.h"
#include "main_game_scene.h"

// file to have functions for organizing enemy creation

EntityID EnemySpawner::SpawnEnemyAt(SceneBase* scene, int x, int y, ENEMY_TYPE type, bool debug)
{
    EntityID enemy = scene->RegisterEntity();
    EnemyArchetype enemy_archetype = g_enemy_archetypes[type];

    ADD_Collider(enemy, 96, 1, 0);
    ADD_Transform(enemy,
        (float)x, 
        (float)y,
        0.0F,
        enemy_archetype.size_multiplier );
    ADD_Enemy(enemy, enemy_archetype.base_health, enemy_archetype.base_speed);
    ADD_Sprite(enemy, ResourceManager::GetTexture(enemy_archetype.texture));
    g_Engine.componentArrays.Colliders[enemy].radius = g_Engine.componentArrays.Sprites[enemy].height/2; // copy the collider size from the sprite size


    return enemy;

}

