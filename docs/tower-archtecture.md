Brief explanation of how tower works in this game

Concepts:
- Tower: 
    - Attacks (spawns projectiles or something else) when enemies are withing its range.
    - Always has at least one element, and at most three
    - Tower attacks and stats are determined by the tower elements: every different combination of elements is a different attack. Order and repeated does not matter for attack type, but does for stats.
- Attack:
    - Is one or more entities spawned by the tower
    - can be a projectile, aura, hitscan, etc.
- Element:
    - when placed alone in the grid, creates a tower.
    - if placed on an existing tower with available slots, add the element to the tower
    - repeated elements does not change the tower, but change tower stats
- Projectile: 
    - entity spawned by towers based on element combination
    - used to kill enemies



Code plan:

enum ELEMENT {
    E_NONE,
    E_FIRE_1,
    E_WATER_2.
    E_AIR_3,
    E_ELECTRIC_4,
    E_EARTH_5
}

struct TowerComponent : BaseComponent {
    ELEMENT elements[3];    // note: they are always sorted in element order

    /* these are determined by the elements above -
    int range;              // radius
    float attackCD;         // attack cooldown, measured in seconds
    float currCD;

    // TBD - data that creates projectile?
    int damage;
    PROJECTILE_TYPE projectile_type; // projectile type is a result of the element combination
    int explosion_radius;            // possibly ommited by projectile type
    int speed;                       // possibly ommited by projectile type
    */ 

    // stats
    long int dealt_damage;
    int enemy_kills;
}

struct ProjectileComponent : BaseComponent {
    EntityID originTower; // tags
    // int damage; // removed damage, to have a specific component for damage
}

// note: projectile component is just the damage. the behavior, look, when it deals damage is just a combination of other components, like:
// the bomb projectile would be:
    PROJECTILE
    DAMAGE
    MOVETOXY
    CREATE_EXPLOSION_ON_XY  // will deal damage and create the explosion animation when reaches near XY
    SPRITE

// a jet
    PROJECTILE
    DAMAGE
    JET_ANIMATION 
    HITSCAN

// pellet
    PROJECTILE
    DAMAGE
    MOVETOXY
    COLLIDER
    DAMAGE_ON_COLLISION // just tag

// wind CC
    PROJECTILE
    CC
    TARGET

