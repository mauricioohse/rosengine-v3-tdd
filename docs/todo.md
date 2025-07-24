Main:
- [x] do LifeTimeSystem
- [x] Implement enemy health + enemy life bar, and projectile damage
- [x] implement water (jet, sniper, range)
- [x] Implement earth (pelets) tower
- [x] implement air (hitscan CC) tower
- [x] implement electric (chaining) tower
- [x] implement fire+water tower (AOE jet)
- [x] implement basic sfx
- [x] implement test zone to test tower power on enemy packs
> - [m] implement tower placement system that can merge two towers
- [x] implement sounds for basic projectiles
- [ ] implement different enemy types -> create namespace for enemy creation
- [ ] add to the debug level selection of different enemy types and spawn rate
- [ ] clean up a lot of specific implementation in g_mainGame
- [ ] update the components to not have init function in it
- [ ] change the ECS implementation to be world-based instead of a single instance of entityManager.
    Notes on this at "ECS-improvements-notes.md"
- [ ] Currently the ECS query system is slow bad for performance. We should exchange the queries each loop to be done statically on the first call of the frame, and have a deferred destruction component. current query is O(nxm), n being max entities and M being number of systems.



Mid
- [x] implement tower range on hover
- [ ] when placing tower, it should appear transparent what kind of tower and the range of placement
- [ ] tilemap loading with dynamic dual-grid tilemap see https://www.reddit.com/r/gamemaker/comments/1fifser/autotile_system_with_dual_grid/
- [ ] implement CLI input for debug options and logs, and a logconfig.txt file to load specific types of logging. implement a CLI log command for inspecting entity components mask, data, etc
- [ ] implement circular collision for tower range instead of AABB 
- [m] create ecs_for_each
- [m] remove init and destroy functions from components
- [ ] add a linter
- [ ] add measurement of system execution time
- [ ] separate the render system in smaller systems (needed for when ECS world separation happens)



last (debatable if needed):
- [ ] implement tower data loading from data files instead of hardcoded
- [ ] do DamageSystem and component?




