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
- [m] implement tower placement system that can merge two towers
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
- [x] implement tower range when holding left shift
- [ ] when placing tower, it should appear transparent what kind of tower and the range of placement


last:
- [ ] clean projectile component data
- [ ] implement circular collision for tower range instead of AABB 
- [ ] do DamageSystem and component?
- [ ] create ecs_for_each
- [ ] remove init and destroy functions from components



