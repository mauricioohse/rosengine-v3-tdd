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


// level lookup table
enum LEVEL_ID {
    LEVEL_1,
    LEVEL_MAX
};

struct LevelWaves {
    Wave* waves;
    int wave_count;
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
