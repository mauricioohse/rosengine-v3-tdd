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
const SDL_Color SDL_RED = {255, 0, 0, 255};
const SDL_Color SDL_GREEN = {0, 255, 0, 255};
const SDL_Color SDL_BLUE = {0, 0, 255, 255};
const SDL_Color SDL_WHITE = {255, 255, 255, 255};
const SDL_Color SDL_BLACK = {0, 0, 0, 255};
const SDL_Color SDL_YELLOW = {255, 255, 0, 255};
const SDL_Color SDL_CYAN = {0, 255, 255, 255};
const SDL_Color SDL_MAGENTA = {255, 0, 255, 255};
const SDL_Color SDL_GRAY = {128, 128, 128, 255};
const SDL_Color SDL_ORANGE = {255, 165, 0, 255};
const SDL_Color SDL_PURPLE = {128, 0, 128, 255};
const SDL_Color SDL_BROWN = {165, 42, 42, 255};

// lookup table
static EnemyArchetype g_enemy_archetypes[] = {
    {80, 50, .2f, TEXTURE_BOX_ENEMY},        // ENEMY_BASIC_I
    {120, 50, .2f, TEXTURE_BOX_ENEMY},        // ENEMY_BASIC_II
    {180, 50, .2f, TEXTURE_BOX_ENEMY},        // ENEMY_BASIC_III
    {40, 50, .1f, TEXTURE_BOX_ENEMY, SDL_RED},         // ENEMY_FODDER_I
    {60, 50, .12f, TEXTURE_BOX_ENEMY, SDL_RED},        // ENEMY_FODDER_II
    {90, 50, .15f, TEXTURE_BOX_ENEMY, SDL_RED},        // ENEMY_FODDER_III
    {50, 125, .15f, TEXTURE_BOX_ENEMY, SDL_CYAN},        // ENEMY_RUNNER_I
    {50, 175, .15f, TEXTURE_BOX_ENEMY, SDL_CYAN},        // ENEMY_RUNNER_II
    {80, 250, .15f, TEXTURE_BOX_ENEMY, SDL_CYAN},        // ENEMY_RUNNER_III
    {500, 30, .3f, TEXTURE_BOX_FAT_ENEMY, SDL_BLUE},   // ENEMY_FAT_I
    {1000, 30, .5f, TEXTURE_BOX_FAT_ENEMY, SDL_BLUE},  // ENEMY_FAT_II
    {1000, 60, .6f, TEXTURE_BOX_FAT_ENEMY, SDL_BLUE}, // ENEMY_FAT_III
    {5000, 30, 1.f, TEXTURE_BOSS}, // ENEMY_FAT_III
    // note: the order needs to be the same as in ENEMY_TYPE.
};

struct EnemyGroup {
    ENEMY_TYPE enemy_type;
    int count;
    float spawn_interval;
    float delay_after_group;
};

static EnemyGroup g_enemy_groups[]=
{   // todo: this sucks as it requires equal order as above. group and enemy type should probably be the same thing tbh 
    {ENEMY_BASIC_I, 9, 1.f},
    {ENEMY_BASIC_II, 15, 1.f},
    {ENEMY_BASIC_III, 15, 1.f},
    {ENEMY_FODDER_I, 40, .25f},
    {ENEMY_FODDER_II, 60, .125f},
    {ENEMY_FODDER_III, 90, .05f},
    {ENEMY_RUNNER_I, 5, 1.f},
    {ENEMY_RUNNER_II, 5, 1.f},
    {ENEMY_RUNNER_III, 7, 1.f},
    {ENEMY_FAT_I, 3, 2.f},
    {ENEMY_FAT_II, 3, 2.f},
    {ENEMY_FAT_III, 3, 2.f},
    {ENEMY_BOSS, 1, 2.f},
};

namespace EnemySpawner
{
    EntityID SpawnEnemyAt(SceneBase *scene, int x, int y, ENEMY_TYPE type, bool debug = false);

}
