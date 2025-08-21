#include "systems.h"
#include "scene_manager.h"
#include "enemy_spawner.h"


// each round may be a series of enemy waves
// struct EnemyGroup {
//     ENEMY_TYPE enemy_type;
//     int count;
//     float spawn_interval;     // seconds between spawning each enemy in group
//     float delay_after_group;  // seconds to wait after this group finishes
// };

struct Wave {
    EnemyGroup* groups;
    int group_count;
    float delay_before_wave;  // seconds before this wave starts
};

// level 1 wave definitions
static EnemyGroup level1_wave1_groups[] = {
    g_enemy_groups[ENEMY_BASIC_I],
    g_enemy_groups[ENEMY_BASIC_I],
    g_enemy_groups[ENEMY_BASIC_I]
};

static EnemyGroup level1_wave2_groups[] = {
    
    g_enemy_groups[ENEMY_FODDER_I],
    g_enemy_groups[ENEMY_BASIC_I]
};

static EnemyGroup level1_wave3_groups[] = {
    g_enemy_groups[ENEMY_FAT_I],
    g_enemy_groups[ENEMY_FODDER_I],
    g_enemy_groups[ENEMY_FAT_I],
    g_enemy_groups[ENEMY_FODDER_I],
};

static EnemyGroup level1_wave4_groups[] = {
    g_enemy_groups[ENEMY_FAT_I],
    g_enemy_groups[ENEMY_FODDER_III],
};


static EnemyGroup level1_wave5_groups[] = {
    g_enemy_groups[ENEMY_FAT_II],
    g_enemy_groups[ENEMY_RUNNER_II],
};


static EnemyGroup level1_wave6_groups[] = {
    g_enemy_groups[ENEMY_FAT_III],
    g_enemy_groups[ENEMY_FODDER_II],
    g_enemy_groups[ENEMY_FODDER_II],
};


static EnemyGroup level1_wave7_groups[] = {
    g_enemy_groups[ENEMY_FAT_I],
    g_enemy_groups[ENEMY_RUNNER_II],
    g_enemy_groups[ENEMY_BASIC_III],
    g_enemy_groups[ENEMY_FODDER_II],
};



// wave lookup table for level 1
static Wave level1_waves[] = {
    {level1_wave1_groups, 3, 0.0f},     // wave 1: 3 groups
    {level1_wave2_groups, 2, 2.0f},     // wave 2: 2 groups
    {level1_wave3_groups, 4, 2.0f},     // wave 3: 4 groups
    {level1_wave4_groups, 2, 2.0f},     // wave 4: 2 groups
    {level1_wave5_groups, 2, 2.0f},     // wave 5: 2 groups
    {level1_wave6_groups, 3, 2.0f},     // wave 6: 3 groups
    {level1_wave7_groups, 4, 2.0f},     // wave 7: 4 groups
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
    {level1_waves, 7},  // LEVEL_1: 7 waves
    // add more levels here
};

enum WAVE_STATE {
    WAVE_STATE_SPAWNING,        // actively spawning enemies
    WAVE_STATE_WAITING,         // waiting between waves  
    WAVE_STATE_TOWER_SELECTION, // player must select and place a tower
    WAVE_STATE_COMPLETE         // all waves finished
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
    WAVE_STATE state;
};

// functions
void WaveSystem_Init(LEVEL_ID level);
void WaveSystem_Update(float deltaTime, SceneBase* scene);
void WaveSystem_StartNextWave();  // manual trigger
bool WaveSystem_AllWavesComplete();
bool WaveSystem_IsInTowerSelection();
