#include "enemy_spawner.h"
#include "engine.h"
#include "main_game_scene.h"

// file to have functions for organizing enemy creation

EntityID EnemySpawner::SpawnEnemyAt(SceneBase* scene, int x, int y, ENEMY_TYPE type, bool debug )
{
    EntityID enemy = scene->RegisterEntity();
    EnemyArchetype enemy_archetype = g_enemy_archetypes[type];

    ADD_Transform(enemy,
        (float)x, 
        (float)y,
        0.0F,
        enemy_archetype.size_multiplier );
    auto en = ADD_Enemy(enemy, enemy_archetype.base_health, enemy_archetype.base_speed, type);
    auto sp = ADD_Sprite(enemy, ResourceManager::GetTexture(enemy_archetype.texture));
    sp->colorMod = enemy_archetype.color;
    auto coll = ADD_Collider(enemy, 96, 1, 0);
    coll->radius = sp->height / 2; // copy the collider size from the sprite size

    return enemy;

}

