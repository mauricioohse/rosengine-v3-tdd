# RoseNgine future improvements

- [ ] i just added a bunch of engine.h to all systems. I believe that should not be required =( that makes changes in engine.h to need to recompile all systems, which does not seem smart.
- [ ] finish converting all components to xcomponent() format
- [ ] create JSON/CSV-based object loading
    > there should be a way to easily load batches of entities from json or CSV files for creating levels/scenes
- [ ] loading tilemap from txt file and tiles from tilemap
- [ ] Scene management
    > currently no scene management in place
- [ ] Discuss header archtecture
    > current clean build takes several seconds
    > changes in components or systems due to header inclusions make recompile almost all components and systems
- [ ] main menu
    > need to think on archtecture changes for this. Scene management should be done first.
- [ ] Create archtecture changes in systems for pausing
    > should every system contain a "stop on pause" variable?
- [ ] music and SFX volume setting on pause/main menu
- [ ] discuss current render system
    > sprites system should have a z value for z positioning
    > 3d rotations support would be cool, for later moment
- [ ] create better way to manage animation component
    > currently for objects animated I am just changing the sprite manually in the systems
    > feels like I should load all sprites and just change a variable to choose which one is the current or something
- [ ] create a better way to get component from an entity
    > current way is way to verbose
- [ ] Discuss current ECS archtecture
  - [ ] create component 2d array instead of single components (how xcomponent() is doing now)
  - [ ] create auxiliary functions to get entities with certain components list only once insted of every system going through all entities everytime
- [ ] Enhance component init macro -
    > current component init macros cant take default values (current automatic generated macro requires explicit setting of all arguments, see xcomponent())
    > option can simply be add a wrapper macro, but where would that go? there should be a more sofisticated solution
- [ ] saving of scoreboard locally (textfile)
- [ ] saving of scoreboard online (scoreSpace?)
- [ ] text > have a specific textManager class for creating texts, text boxes, alignment with left/middle/right, border selection
- [ ] Make physics system flexible
    > current physics system is not flexible and is tailored for a specific game. a good physics component and system should be flexible - friction, gravity, momentum, etc, all configurable
- [ ] Make controller system flexible
    > WASD controller should be more configurable
    > WASD controller should work with arrowkeys too
- [ ] Create logging system
    > could have a debug logging system that logs file function and line when printing, also enable log levels (trace, warning, error, critical, etc)
- [ ] create save states
- [ ] add networking capabilities
- [ ] 3d rendering
- [ ] Create resource manager to only load resources in parts
