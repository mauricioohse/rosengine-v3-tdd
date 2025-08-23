#include "wave_system.h"
#include "enemy_spawner.h"
#include "main_game_scene.h"
#include "grid.h"
#include "engine.h"
#include "tower_placement.h"

static WaveSystemState g_wave_ctx = {};


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


static EnemyGroup level1_wave8_groups[] = {
    g_enemy_groups[ENEMY_BOSS],
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
    {level1_wave8_groups, 1, 2.0f},     // wave 8: 1 groups (boss wave)
};

void WaveSystem_Init(LEVEL_ID level)
{
    g_wave_ctx.current_level = level;
    g_wave_ctx.current_wave_index = 0;
    g_wave_ctx.current_group_index = 0;
    g_wave_ctx.enemies_spawned_in_group = 0;
    g_wave_ctx.spawn_timer = 0.0f;
    g_wave_ctx.wave_delay_timer = 0.0f;
    g_wave_ctx.auto_wave_timer = 0.0f;
    g_wave_ctx.wave_active = false;
    g_wave_ctx.waiting_for_next_wave = false;
    g_wave_ctx.state = WAVE_STATE_TOWER_SELECTION;
    
    printf("wave system initialized for level %d\n", level);
}

bool WaveSystem_IsInTowerSelection()
{
    return (g_wave_ctx.state ==  WAVE_STATE_TOWER_SELECTION);
}

void WaveSystem_Update(float deltaTime, SceneBase* scene)
{
    if (g_wave_ctx.current_level >= LEVEL_MAX) {
        DO_ONCE(printf("no wave implemented for current level!\n"));
        return; // no waves defined for this level
    }
    
    if(g_wave_ctx.state ==  WAVE_STATE_TOWER_SELECTION){
        // wait for tower_placement to resolve the tower placement
        return;
    }

    LevelWaves* level_waves = &g_level_waves[g_wave_ctx.current_level];
    
    if (g_wave_ctx.current_wave_index >= level_waves->wave_count) {
        return; // all waves complete
    }
    
    Wave* current_wave = &level_waves->waves[g_wave_ctx.current_wave_index];
    
    // handle wave delay timer
    if (g_wave_ctx.wave_delay_timer >= 0.0f) {
        g_wave_ctx.wave_delay_timer -= deltaTime;
        if (g_wave_ctx.wave_delay_timer <= 0.0f) {
            g_wave_ctx.wave_active = true;
            g_wave_ctx.spawn_timer = 0.0f; // start spawning immediately
        }
        return;
    }
    
    // handle wave delay before starting
    if (!g_wave_ctx.wave_active && !g_wave_ctx.waiting_for_next_wave) {
        g_wave_ctx.wave_delay_timer = current_wave->delay_before_wave;
        return;
    }
    
    // handle spawning within current group
    if (g_wave_ctx.wave_active && g_wave_ctx.current_group_index < current_wave->group_count) {
        EnemyGroup* current_group = &current_wave->groups[g_wave_ctx.current_group_index];
        
        // spawn timer logic
        if (g_wave_ctx.enemies_spawned_in_group < current_group->count) {
            g_wave_ctx.spawn_timer -= deltaTime;
            
            if (g_wave_ctx.spawn_timer <= 0.0f) {
                // spawn enemy
                Point starting_point = Grid::GetMonsterPathPoint(0);

                int spawn_x = starting_point.x;
                int spawn_y = starting_point.y;
                
                EnemySpawner::SpawnEnemyAt(scene, spawn_x, spawn_y, current_group->enemy_type, false);
                
                g_wave_ctx.enemies_spawned_in_group++;
                g_wave_ctx.spawn_timer = current_group->spawn_interval;
                
                printf("spawned enemy %d/%d in group %d, wave %d\n", 
                       g_wave_ctx.enemies_spawned_in_group, current_group->count,
                       g_wave_ctx.current_group_index, g_wave_ctx.current_wave_index);
            }
        } else {
            // group complete, move to next group or wave
            g_wave_ctx.current_group_index++;
            g_wave_ctx.enemies_spawned_in_group = 0;
            
            if (g_wave_ctx.current_group_index >= current_wave->group_count) {
                // wave complete
                g_wave_ctx.current_wave_index++;
                g_wave_ctx.current_group_index = 0;
                g_wave_ctx.wave_active = false;
                g_wave_ctx.waiting_for_next_wave = true;
                g_wave_ctx.auto_wave_timer = 0.0f; // start auto timer
                
                printf("wave %d complete\n", g_wave_ctx.current_wave_index - 1);
            } else {
                // delay before next group in same wave
                EnemyGroup* next_group = &current_wave->groups[g_wave_ctx.current_group_index];
                g_wave_ctx.spawn_timer = next_group->delay_after_group;
            }
        }
    }
    
    // handle automatic wave start after 10 seconds
    if (g_wave_ctx.waiting_for_next_wave) {
        g_wave_ctx.auto_wave_timer += deltaTime;
        if (g_wave_ctx.auto_wave_timer >= 10.0f) {
            g_wave_ctx.state = WAVE_STATE_TOWER_SELECTION;
            g_wave_ctx.auto_wave_timer = 0.0f;
        }
    }
}

static EntityID wave_text = INVALID_ENTITY;
static int last_wave_in_text = 0;

void WaveSystem_StartNextWave()
{
    // handle both initial tower selection and between-wave cases
    if (g_wave_ctx.waiting_for_next_wave || g_wave_ctx.state == WAVE_STATE_TOWER_SELECTION) {
        g_wave_ctx.waiting_for_next_wave = false;
        g_wave_ctx.wave_active = false; // will trigger wave delay timer
        g_wave_ctx.wave_delay_timer = 0.0f; // reset delay timer to trigger immediate processing
        g_wave_ctx.auto_wave_timer = 0.0f; // reset auto timer
        g_wave_ctx.state = WAVE_STATE_SPAWNING;

        char text[40];
        snprintf(text, sizeof(text),"current wave: %d/7", g_wave_ctx.current_wave_index );
        printf(text);
        printf("\n");

        // create a red text in the middle of the screen saying starting next wave, with 3 second lifetime
        if (!g_Engine.entityManager.IsEntityValid(wave_text))
        {
            wave_text = g_mainGame.RegisterEntity();
            ADD_Transform(wave_text, 1440, 50, 0, 1.0f);
            ADD_Text(wave_text, ResourceManager::GetFont(FONT_FPS), text);
        }
        else
        {
            // if wave changed, update text
            if (g_wave_ctx.current_wave_index != last_wave_in_text)
            {
                last_wave_in_text = g_wave_ctx.current_wave_index;
                auto comp = GET_Text(wave_text);
                snprintf(comp->text, sizeof(comp->text), text);
                comp->isDirty =  true;
            }
        }
        
    }
}

bool WaveSystem_AllWavesComplete()
{
    if (g_wave_ctx.current_level >= LEVEL_MAX) {
        return true;
    }

    LevelWaves* level_waves = &g_level_waves[g_wave_ctx.current_level];
    
    // check if all waves in current level are complete
    if (g_wave_ctx.current_wave_index >= level_waves->wave_count) {
        // if this was the last level, we're done
        if (g_wave_ctx.current_level + 1 >= LEVEL_MAX) {
            return true;
        }
        
        // otherwise advance to next level
        g_wave_ctx.current_level = (LEVEL_ID)(g_wave_ctx.current_level + 1);
        g_wave_ctx.current_wave_index = 0;
        g_wave_ctx.current_group_index = 0;
        g_wave_ctx.enemies_spawned_in_group = 0;
        g_wave_ctx.wave_active = false;
        g_wave_ctx.waiting_for_next_wave = false;
        g_wave_ctx.state = WAVE_STATE_TOWER_SELECTION;
        
        printf("advanced to level %d\n", g_wave_ctx.current_level);
        
        // check again with new level
        return WaveSystem_AllWavesComplete();
    }
    
    return false;
}
