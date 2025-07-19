Main:
- [x] do LifeTimeSystem
- [x] Implement enemy health + enemy life bar, and projectile damage
- [x] implement water (jet, sniper, range)
- [x] Implement earth (pelets) tower
- [x] implement air (hitscan CC) tower
- [x] implement electric (chaining) tower
- [x] implement fire+water tower (AOE jet)
- [x] implement basic sfx
- [ ] implement test zone to test tower power on enemy packs
- [ ] implement tower placement system that can merge two towers
- [ ] implement sounds for basic projectiles
- [ ] implement 
- [ ] change the ECS implementation to be world-based instead of a single instance of entityManager.
    Notes on this:
    - Currently, we have one g_engine which has global entityManager, componentArray, SceneManager and systemManager
        - each scene then has a vector of which entities it actually contains, which is duplicated info from the entityManager
        - and each scene calls ALL registered systems in its update
        - this causes a couple of issues:
            - first, on the systems itself, when it generates I have to create the entity directly on the scene. If I am using g_mainGame always as the scene that contains the game, that would be ok, but that brings a lot of complexity in the implementation of g_mainGame
            - because I would need to load all levels in the g_mainGame, for example. which may be fine.
            - so to add levels, I would need:
                - specific data on g_mainGame that determines which level I am currently, and decides to init or delete all entities based on it being loaded or not

        - A different approach would be to, instead of having ONE g_mainGame, we just adapt everything to be based on ecs_worlds
        - with ecs_worlds, ecs_worlds is just the data that contain .activeEntities[], .componentsMasks[], and .componentsdata
        - then, we still have the scenes, but each scene has its own ecs_world.
        - this solves that if I want to delete all entities in a scene I can just call ecs_destroy_world(specific_scene->world) 
        - "problems"
            - each scene will have to register its own systems. today all systems go by default to all scenes.
            - it seems that we should have a different word to differentiate a world of entities and a scene. the game scene should have a world loaded form alevel. the game scene should always load a world and iterate all relevant game systems, while a menu scene would load a specific menu UI world and only systems related to that.
            - how would the archtecture look like in that case?
            
            - SceneManager
                - has the sceneStack
                - functions: destroyScene(stack ,*scene), insert(stack, *Scene)
                - calls:
                    - scene->update() only for the active scenes
                    - scene->draw() for UPDATE_AND_DRAW and ONLY_DRAW scenes, from bottom to top of the stack
            - scenes: menu, game, pause
                scenes: init(), update(), draw(), possibly destroy()
                    -> on init, load the scene world
                    -> on update(), update the scene and send the needed systems
                scenes data: state (UPDATE_AND_DRAW, ONLY_DRAW, TO_DESTROY)
                - scenes will load a world on init.

            - worlds: menu_ui_world, game_world, pause_world 
                > game_world will be loaded with function:
                    load_level(game_world or game_scene, csv_path)
                        - how would the grid work? well, the grid is drawn by the game scene, so it would be somewhat agnostic!
                - the scene management will still be the same.

    in practice, if we dont change nothing in the ECS now, because that doesnt necessarily solves the problem, we would have:

    in the g_mainGame, we would have the following data:
        - levelPath: loads the grid size + enemy path
        - towerPlacePath: loads the towers to be placed (will be only used in debug to quickly test stuff)
        - EnemyWaves::initEnemyPattern: we will place enemies. EnemyWaves::update() will place enemies on the beggining of the path (which he gets from the Grid class)
        - EnemyStats:: -> a system to count how much damage and kills a specific tower has gotten.

        note: we should add a EXIT component that decreases player life when enemy collides there.  



Mid
- [ ] implement tower range on hover
- [ ] when placing tower, it should appear transparent what kind of tower and the range of placement


last:
- [ ] clean projectile component data
- [ ] implement circular collision for tower range instead of AABB 
- [ ] do DamageSystem and component?
- [ ] create ecs_for_each
- [ ] remove init and destroy functions from components

