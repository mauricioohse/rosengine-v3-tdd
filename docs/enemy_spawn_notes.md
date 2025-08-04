some notes on how to implement the enemy spawning and the waves:


general:
    - file enemy_spawn.h has all the enemy types (enum) and function for spawning enemies
    - file wave_system.h has the functions related to how group of enemies and timing works


idea 1:
    - create an entity Spawner with component "EnemySpawner". Enemy spawner has a cooldown and spawn enemies based on that
    - when all enemies are killed, or when user press to send next wave, next wave begin
    - next wave is simply a new entity spawner.
    - for different enemies, need to spawn different spawners ?


idea 2:
    - since we dont intend to have spawning properties be something shared by any other entity, I dont see why use ECS in this case
    - could just be a system that spawns based on a dictonary and what level is loaded


lets go with idea 2.