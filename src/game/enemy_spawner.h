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
    {150, 50, .1f, TEXTURE_BOX_ENEMY, 0, 0, 0},     // ENEMY_BASIC - more hp
    {120, 80, .12f, TEXTURE_BOX_ENEMY, 0, 0, 0},    // ENEMY_BASIC_FAST - faster, less hp
    {400, 30, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_SLOW - tanky
    {350, 55, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_NORMAL_SPEED - balanced tank
    {300, 85, .4f, TEXTURE_BOX_FAT_ENEMY, 0, 0, 0}, // ENEMY_FAT_FAST - fast tank
    // note: the order needs to be the same as in ENEMY_TYPE.
};

namespace EnemySpawner
{
    EntityID SpawnEnemyAt(SceneBase *scene, int x, int y, ENEMY_TYPE type, bool debug = false);

}
