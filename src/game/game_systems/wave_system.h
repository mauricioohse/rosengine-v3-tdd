#include "systems.h"
#include "scene_manager.h"
#include "enemy_spawner.h"


// each round may be a series of enemy waves
struct EnemyGroup {
    ENEMY_TYPE enemy_type;
    int count;
    float spawn_interval;     // seconds between spawning each enemy in group
    float delay_after_group;  // seconds to wait after this group finishes
};

struct Wave {
    EnemyGroup* groups;
    int group_count;
    float delay_before_wave;  // seconds before this wave starts
};

// level 1 wave definitions
static EnemyGroup level1_wave1_groups[] = {
    {ENEMY_BASIC, 6, 1.2f, 2.0f},        // 6 basic enemies, slower spawn
    {ENEMY_BASIC_FAST, 4, 1.0f, 1.0f},   // 4 fast enemies
    {ENEMY_BASIC, 3, 0.8f, 0.0f},        // 3 more basic
};

static EnemyGroup level1_wave2_groups[] = {
    {ENEMY_BASIC, 8, 0.8f, 1.5f},        // 8 basic enemies, faster spawn
    {ENEMY_BASIC_FAST, 5, 1.0f, 2.0f},   // 5 fast enemies
    {ENEMY_FAT_SLOW, 1, 3.0f, 0.0f},     // 1 tank
};

static EnemyGroup level1_wave3_groups[] = {
    {ENEMY_BASIC, 12, 0.6f, 2.0f},       // more basic enemies
    {ENEMY_FAT_SLOW, 2, 2.5f, 1.5f},     // 2 tanks
    {ENEMY_BASIC_FAST, 6, 0.8f, 0.0f},   // 6 fast enemies
};

// wave lookup table for level 1
static Wave level1_waves[] = {
    {level1_wave1_groups, 1, 0.0f},      // 1 group, start immediately
    {level1_wave2_groups, 2, 3.0f},      // 2 groups, 3 sec delay
    {level1_wave3_groups, 3, 4.0f},      // 3 groups, 4 sec delay
};

// level lookup table
enum LEVEL_ID {
    LEVEL_1,
    LEVEL_MAX
};

struct LevelWaves {
    Wave* waves;
    int wave_count;
};

static LevelWaves g_level_waves[] = {
    {level1_waves, 3},  // LEVEL_1: 3 waves
    // add more levels here
};

// system state
struct WaveSystemState {
    LEVEL_ID current_level;
    int current_wave_index;
    int current_group_index;
    int enemies_spawned_in_group;
    float spawn_timer;
    float wave_delay_timer;
    float auto_wave_timer;    // timer for automatic wave start after 10 seconds
    bool wave_active;
    bool waiting_for_next_wave;
};

// functions
void WaveSystem_Init(LEVEL_ID level);
void WaveSystem_Update(float deltaTime, SceneBase* scene);
void WaveSystem_StartNextWave();  // manual trigger
bool WaveSystem_AllWavesComplete();

