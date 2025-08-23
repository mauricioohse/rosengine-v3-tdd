#pragma once
#include "systems.h"
#include "scene_manager.h"
#include "ecs_types.h"


struct EnemyArchetype {
    int base_health;
    int base_speed;
    float size_multiplier;
    TextureID texture;
    SDL_Color color = {255,255,255,255};
    // behavioral flags
    int flies = 0;
    int armored = 0;
    int splits_on_death = 0;
};


enum ENEMY_TYPE {
    ENEMY_BASIC_I,
    ENEMY_BASIC_II,
    ENEMY_BASIC_III,
    ENEMY_FODDER_I,
    ENEMY_FODDER_II,
    ENEMY_FODDER_III,
    ENEMY_RUNNER_I,
    ENEMY_RUNNER_II,
    ENEMY_RUNNER_III,
    ENEMY_FAT_I,
    ENEMY_FAT_II,
    ENEMY_FAT_III,
    ENEMY_BOSS,
    ENEMY_LAST_VALUE // keep as last
    // whenever creating a new enemy type, also add a comment in g_enemy_archetypes matching it!
};

struct EnemyGroup {
    ENEMY_TYPE enemy_type;
    int count;
    float spawn_interval;
    float delay_after_group;
};

extern EnemyGroup g_enemy_groups[];

namespace EnemySpawner
{
    EntityID SpawnEnemyAt(SceneBase *scene, int x, int y, ENEMY_TYPE type, bool debug = false);

}
