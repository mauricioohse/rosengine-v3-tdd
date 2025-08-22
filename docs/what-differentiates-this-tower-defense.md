
what differentiates this tower defense from the thousands of others?

Also in here: what will be the prototype level for us to test if the idea is good?


First:

# Why is classic tower defense interesting to play?
- there are many different approaches to tower defense
- the most classic ones work as some sort of puzzle + discovery game
    - usually in the begining there are very few tower types and enemy types
    - so the player needs to choose between the options and maximize efficiency on both placement and economy management
    - each tower has its strengths and weakness, and the player needs to consider that to choose what, when and where to place the tower
    - as the game progresses, more enemy types and more towers are available

that is as generic as it gets, but here are some differentiators in specific games:
-> in bloons TD, 
    - there are a huge amount of towers, and a huge amount of upgrades (three path per tower, up to level 5 on one path + 2 level on other). this brings an enourmous amount of tower diversity and power.
    - fully leveled up towers are very fun to get to know what they do
    - they also have like 10 challenge per map, and 50 maps. so a lot of gameplay
    - note: the waves are always the same, what changes are the maps characteristics
    - a map can take 30 minutes from start to finish, and once you finish, that is that. if you die you need to restart

-> in orcs must die:
    - from what I understood, it is mostly a puzzle + build up of strength between levels
    - after each level you get a checkpoint. you first build all the defenses, then the enemies come. it is less about many waves, and more about planning + defending later
    - it is first person and 3D
    - the player can also hit enemies

-> gemcraft
    - gemcraft has a bunch of different gems and combinations
    - it is more about puzzle (discovering what gem combination is good against certain enemy type), but the towers are not very distinct (more continuous changes)
    - the economy is also the health, which is pretty interesting

-> our game!
    - we are still deciding on the mechanics, but it seems closer to gemcraft (since you can merge elements to get new elements), but like magicka, different element combination should be very different towers!
    - so far we dont intend to put economy, it has a draft-style to get more elements instead.
    - but it seems something is missing to make it interesting and not just a classic TD. how?
    - I believe there are two axis that we have not decided yet that is very important to continue the development:
        ## Level/run archetype axis:
            - Rogue-like archetype: 
                - a run would be three maps. each round the player chooses some options (normal enemy, elite, shop, event ). then something happen. for normal enemies and elite, it would be 5 to 10 waves.
                - at the end of the normal enemy waves and elite, player gets some rewards: more elements, possibly gold for the shop (where they can buy and remove elements/curses/etc). on elite fights, get a random relic
                - create many different relics that when comboed correctly, can give a big advantage or a very different build then usual
                - aimed to have 1h long runs in normal runs
            - Challenge archetype
                - levels would be similar to bloons tower defense
                - levels could evolve after certain waves, changing path, destroying towers, etc
                - each level would be a puzzle (maybe the element draft would be set and not random)
                - you could have many different challenges per level (limit the number of elements, increase speed, like BTD does)
                - a run would be finishing a map from start to finish, possibly with a boss in the end, about 30-40 minutes

        ## Spell building, and inventory axis
            - Magicka spell archetype
                - each element combination would be a totally different spell. part of the fun is discovering all the possible spell combinations.


            - Noita approach
                - In noita, there are "projectile spells" and "modifier spells". a wand can cast the spell based on the combination of its spells
                - this allows some fun combos and a lot theory crafting
                - in noita by default you cant rearrange spell, it requires a skill to do that.
                - there are many ways to implement:
                    > Approach one: repeated gems always gives the same modifier.
                        - i.e. fire always increases the AOE, water increases range, electro increases projectile quantity, wind increases slow/knockback, earth increases attack speed, etc

                    > Approach two: there would be elemental gems and modifier gems
                        - i.e. FIRE + WATER gem creates a sniper bomb, and you could add a "faster attack speed" modifier gem, or things more interesting than that.



Ideas for the first alpha prototypes so we can test what idea is fun:


(1) Only three element
    - Implement a basic level with 10 waves. after every wave, player can choose only betwen fire, water and air.
    - implement all combinations for fire, water and air
    - make the waves test something each time: single target dps, group dps, range, crowd control


(2) All elements allowed, drafting
    - implement a basic level with 10 waves. after every wave, player needs to draft from a random element.
    - makes the game more random
    - harder to balance but brings novelty each time you play



Designing (1)

Towers:
FIRE: AOE, good against enemies with 15 spacing. 20 dps.
WATER: 40 dps, single target. slow, big range.
AIR: single enemy CC (25%)

duals:
FIRE-FIRE: big AOE (20 spacing). 30 dps.
FIRE-WATER: small AOE (good against 10 spacing), small range, slow, 50 DPS
FIRE-AIR: small AOE CC (10 spacing). small range.
WATER-WATER: single target, best DPS and range. very slow, a lot of overkill. 100 dps
WATER-AIR: better single enemy CC (50%)
AIR-AIR: gives knockback. 25% CC still. starts grouping enemies
FIRE-FIRE-FIRE: explosion around the tower. good up to 30 spacing. 40 DPS.
FIRE-FIRE-WATER: increase FIRE-WATER AOE size. 60 DPS
FIRE-FIRE-AIR: increase FIRE-AIR AOE size
WATER-WATER-FIRE: same as WATER-FIRE, but with small AOE
WATER-WATER-WATER: highest DPS, even more overkill. 150 DPS.
WATER-WATER-AIR: same as WATER-AIR, but bigger range + damage
AIR-AIR-AIR
AIR-AIR-WATER
AIR-AIR-FIRE




todo:
[x] - basic archetypes
[ ] - boss (with enemy spawner)
[x] - remaining dual towers
[x] - FIRE + WATER + AIR
[x] - implement the waves, text when wave starting
[x] - implement the reward element selection
[ ] - sound when wave ends and when tower is placed
[ ] - overhaul in wave system (eventually)
[ ] - towers need to be more differentiable between them
[ ] - targetting needs to be improved, always target target that is closer to the end
[ ] - on mouse over, tower should tell damage + kills
[ ] - enemy collor based on what is best against it (red for water, ice-blue for fodder, green for wind why not)