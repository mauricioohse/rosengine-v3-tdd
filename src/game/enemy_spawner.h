#pragma once
#include "systems.h"
#include "scene_manager.h"
#include "ecs_types.h"


struct EnemyArchetype {
    int base_health;
    int base_speed;
    float size_multiplier;
    TextureID texture;
    // behavioral flags
    int flies = 0;
    int armored = 0;
    int splits_on_death = 0;
};


enum ENEMY_TYPE {
    ENEMY_BASIC,
    ENEMY_BASIC_FAST,
    ENEMY_FAT_SLOW,
    ENEMY_FAT_NORMAL_SPEED,
    ENEMY_FAT_FAST,
    ENEMY_LAST_VALUE // keep as last
    // whenever creating a new enemy type, also add a comment in g_enemy_archetypes matching it!
};


// lookup table
static EnemyArchetype g_enemy_archetypes[] = {
    {100, 50, .1f, TEXTURE_BOX_ENEMY, 0, 0, 0},     // ENEMY_BASIC
    {100, 75, .15f, TEXTURE_BOX_ENEMY, 0, 0, 0},    // ENEMY_BASIC_FAST
    {300, 25, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_SLOW
    {300, 50, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_NORMAL_SPEED
    {300, 75, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_FAST
    // note: the order needs to be the same as in ENEMY_TYPE.
};

namespace EnemySpawner
{
    EntityID SpawnEnemyAt(SceneBase *scene, int x, int y, ENEMY_TYPE type, bool debug = false);

}
