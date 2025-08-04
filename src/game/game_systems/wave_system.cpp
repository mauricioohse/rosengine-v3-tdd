#include "wave_system.h"
#include "enemy_spawner.h"
#include "main_game_scene.h"
#include "grid.h"

static WaveSystemState g_wave_state = {};

void WaveSystem_Init(LEVEL_ID level)
{
    g_wave_state.current_level = level;
    g_wave_state.current_wave_index = 0;
    g_wave_state.current_group_index = 0;
    g_wave_state.enemies_spawned_in_group = 0;
    g_wave_state.spawn_timer = 0.0f;
    g_wave_state.wave_delay_timer = 0.0f;
    g_wave_state.auto_wave_timer = 0.0f;
    g_wave_state.wave_active = false;
    g_wave_state.waiting_for_next_wave = false;
    
    printf("wave system initialized for level %d\n", level);
}

void WaveSystem_Update(float deltaTime, SceneBase* scene)
{
    if (g_wave_state.current_level >= LEVEL_MAX) {
        DO_ONCE(printf("no wave implemented for current level!\n"));
        return; // no waves defined for this level
    }
    
    LevelWaves* level_waves = &g_level_waves[g_wave_state.current_level];
    
    if (g_wave_state.current_wave_index >= level_waves->wave_count) {
        return; // all waves complete
    }
    
    Wave* current_wave = &level_waves->waves[g_wave_state.current_wave_index];
    
    // handle wave delay timer
    if (g_wave_state.wave_delay_timer >= 0.0f) {
        g_wave_state.wave_delay_timer -= deltaTime;
        if (g_wave_state.wave_delay_timer <= 0.0f) {
            g_wave_state.wave_active = true;
            g_wave_state.spawn_timer = 0.0f; // start spawning immediately
        }
        return;
    }
    
    // handle wave delay before starting
    if (!g_wave_state.wave_active && !g_wave_state.waiting_for_next_wave) {
        g_wave_state.wave_delay_timer = current_wave->delay_before_wave;
        return;
    }
    
    // handle spawning within current group
    if (g_wave_state.wave_active && g_wave_state.current_group_index < current_wave->group_count) {
        EnemyGroup* current_group = &current_wave->groups[g_wave_state.current_group_index];
        
        // spawn timer logic
        if (g_wave_state.enemies_spawned_in_group < current_group->count) {
            g_wave_state.spawn_timer -= deltaTime;
            
            if (g_wave_state.spawn_timer <= 0.0f) {
                // spawn enemy
                Point starting_point = Grid::GetMonsterPathPoint(0);

                int spawn_x = starting_point.x;
                int spawn_y = starting_point.y;
                
                EnemySpawner::SpawnEnemyAt(scene, spawn_x, spawn_y, current_group->enemy_type, false);
                
                g_wave_state.enemies_spawned_in_group++;
                g_wave_state.spawn_timer = current_group->spawn_interval;
                
                printf("spawned enemy %d/%d in group %d, wave %d\n", 
                       g_wave_state.enemies_spawned_in_group, current_group->count,
                       g_wave_state.current_group_index, g_wave_state.current_wave_index);
            }
        } else {
            // group complete, move to next group or wave
            g_wave_state.current_group_index++;
            g_wave_state.enemies_spawned_in_group = 0;
            
            if (g_wave_state.current_group_index >= current_wave->group_count) {
                // wave complete
                g_wave_state.current_wave_index++;
                g_wave_state.current_group_index = 0;
                g_wave_state.wave_active = false;
                g_wave_state.waiting_for_next_wave = true;
                g_wave_state.auto_wave_timer = 0.0f; // start auto timer
                
                printf("wave %d complete\n", g_wave_state.current_wave_index - 1);
            } else {
                // delay before next group in same wave
                EnemyGroup* next_group = &current_wave->groups[g_wave_state.current_group_index];
                g_wave_state.spawn_timer = next_group->delay_after_group;
            }
        }
    }
    
    // handle automatic wave start after 10 seconds
    if (g_wave_state.waiting_for_next_wave) {
        g_wave_state.auto_wave_timer += deltaTime;
        if (g_wave_state.auto_wave_timer >= 10.0f) {
            WaveSystem_StartNextWave();
            g_wave_state.auto_wave_timer = 0.0f;
        }
    }
}

void WaveSystem_StartNextWave()
{
    if (g_wave_state.waiting_for_next_wave) {
        g_wave_state.waiting_for_next_wave = false;
        g_wave_state.wave_active = false; // will trigger wave delay timer
        g_wave_state.auto_wave_timer = 0.0f; // reset auto timer
        printf("starting next wave %d\n", g_wave_state.current_wave_index);
    }
}

bool WaveSystem_AllWavesComplete()
{
    if (g_wave_state.current_level >= LEVEL_MAX) {
        return true;
    }
    
    LevelWaves* level_waves = &g_level_waves[g_wave_state.current_level];
    return g_wave_state.current_wave_index >= level_waves->wave_count;
}
