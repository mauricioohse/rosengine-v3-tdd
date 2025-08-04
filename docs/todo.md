Needed for prototype, highest priority:
- [ ] implement different enemy types -> create namespace for enemy creation
- [ ] implement a simple new element selection event
- [ ] implement a health bar + enemy taking out health from the health bar
- [ ] implement a first level, basic wave system
- [ ] add to the debug level selection of different enemy types and spawn rate
- [ ] tilemap loading with dynamic dual-grid tilemap see https://www.reddit.com/r/gamemaker/comments/1fifser/autotile_system_with_dual_grid/


Things we also need to do eventually:
- [ ] create a debug system that check for archetypes, making sure certain combinations of components are not missing (i.e. all towers require one element, or all the components that always require transforms for example) 
- [ ] sprite should only have an enum that points to certain texture so that it is easier to type (only call ResourceManager:: in the render system instead of needing to call it on the sprite creation)
- [ ] when placing tower, it should appear transparent what kind of tower and the range of placement
- [ ] implement CLI input for debug options and logs, and a logconfig.txt file to load specific types of logging. implement a CLI log command for inspecting entity components mask, data, etc
- [n] implement circular collision for tower range instead of AABB 
- [ ] add a linter
- [ ] add measurement of system execution time
- [ ] separate the render system in smaller systems (needed for when ECS world separation happens)
- [ ] make a proper targeting system, where the tower can: target the first enemy, the last enemy, the strongest (highest health) or the weakest enemy
- [ ] create sprites of individual elements instead of specific per tower combo


last:
- [ ] implement tower data loading from data files instead of hardcoded
- [ ] do DamageSystem and component?
- [ ] do "DeleteSystem", so we only delete entities at the end of the frame
- [ ] clean up a lot of specific implementation in g_mainGame
- [ ] update the components to not have init function in it
- [ ] Currently the ECS query system is slow bad for performance. We should exchange the queries each loop to be done statically on the first call of the frame, and have a deferred destruction component. current query is O(nxm), n being max entities and M being number of systems.
- [ ] change the ECS implementation to be world-based instead of a single instance of entityManager.
    Notes on this at "ECS-improvements-notes.md"




Done:
- [x] do LifeTimeSystem
- [x] Implement enemy health + enemy life bar, and projectile damage
- [x] implement water (jet, sniper, range)
- [x] Implement earth (pelets) tower
- [x] implement air (hitscan CC) tower
- [x] implement electric (chaining) tower
- [x] implement fire+water tower (AOE jet)
- [x] implement basic sfx
- [x] implement test zone to test tower power on enemy packs
- [x] implement tower placement system that can merge two towers
- [x] implement sounds for basic projectiles
- [x] implement tower range on hover
- [x] create ecs_for_each
- [x] remove init and destroy functions from inside the components to use less memory on components and solve tag components (components taht does not need initialization because has no data)
