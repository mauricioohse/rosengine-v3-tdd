#include "resolve_elements_system.h"
#include "engine.h"
#include "game/tower_placement.h"
#include "systems.h"
#include "scene_manager.h"
#include "game/scenes/main_game_scene.h"

/*
    - Tower name
    - Projectile type and qualitative projectile speed
    - Qualitative damage output
    - Qualitative attack speed
    - Additional CC effect
*/

// the element combination dictionary
static ElementCombo g_element_combinations[] = {
    // single elements - balanced for ~50 dps single target, or 50 dps considering area damage +three targets
    // {{ele1, ele2, ele3},              {projectile_type,      range, cooldown, damage, texture,      {towerName,         projectileType,          damage,                attackSpeed,           specialEffect}, {r,g,b,a}}}
    {{ELE_FIRE, ELE_NONE, ELE_NONE},     {PROJECTILE_BOMB,       125, 2.f,   40,  TEXTURE_BOX,         {"Fire tower",      "Lobbed Explosive Bomb", "Low damage (40)",     "Medium attack speed", "Explosion where bomb lands"}}},
    {{ELE_WATER, ELE_NONE, ELE_NONE},    {PROJECTILE_JET,        250, 3.0f,  150, TEXTURE_BOX_BLUE,    {"Water tower",     "Targeted Jet",          "Medium damage (150)", "Slow attack speed",   "Single target damage"}}},
    {{ELE_EARTH, ELE_NONE, ELE_NONE},    {PROJECTILE_PELLET,     150, 0.5f,  8,   TEXTURE_BOX_EARTH,   {"Earth tower",     "Targeted Pellet",       "Low damage (8)",      "Fast attack speed",   "Single target damage"}}},
    {{ELE_WIND, ELE_NONE, ELE_NONE},     {PROJECTILE_GUST,       150, 2.0f,  0,   TEXTURE_BOX_AIR,     {"Wind tower",      "Targeted Gust",         "No damage (0)",       "Medium attack speed", "Holds target in place"}}},
    {{ELE_ELECTRIC, ELE_NONE, ELE_NONE}, {PROJECTILE_LIGHTNING,  150, 2.5f,  60,  TEXTURE_BOX_ELECTRO, {"Electric tower",  "Targeted Lightning",    "Medium damage (60)",  "Slow attack speed",   "Lightning jumps between targets"}}},

    // dual combinations - stronger but more expensive
    // {{ele1, ele2, ele3},              {projectile_type,               range, cooldown, damage, texture,          {towerName,             projectileType,                 damage,                attackSpeed,           specialEffect},                      {r,g,b,a}}}
    {{ELE_FIRE, ELE_FIRE, ELE_NONE},     {PROJECTILE_BOMB,                125, 1.5f, 60,  TEXTURE_BOX,              {"Fire Fire tower",     "Lobbed Bomb Bigger Area",      "Medium damage (60)",  "Medium attack speed", "Improved damage and attack speed"}, {125, 0, 0, 255}}},
    {{ELE_WATER, ELE_WATER, ELE_NONE},   {PROJECTILE_JET,                 250, 3.0f, 300, TEXTURE_BOX_BLUE,         {"Water Water tower",   "Targeted Stronger Jet",        "High damage (300)",   "Slow attack speed",   "Improved damage and attack speed"}, {0, 0, 125, 255}}},
    {{ELE_WIND, ELE_WIND, ELE_NONE},     {PROJECTILE_GUST,                150, 1.0f, 0,   TEXTURE_BOX_AIR,          {"Wind Wind tower",     "Targeted Faster Gust",         "No damage (0)",       "Fast attack speed",   "Improved attack speed"},            {125, 125, 125, 255}}},
    {{ELE_WATER, ELE_FIRE, ELE_NONE},    {PROJECTILE_JET_BOMB,            150, 3.f,  100, TEXTURE_BOX_FIRE_WATER,   {"Water Fire tower",    "Area of Effect Jet",           "Medium damage (100)", "Slow attack speed",   "Explosive higher damage jet"}}},
    {{ELE_EARTH, ELE_FIRE, ELE_NONE},    {PROJECTILE_BOMB,                200, 0.4f, 12,  TEXTURE_BOX_FIRE_EARTH,   {"Earth Fire tower",    "Targeted Explosive Pellet",    "Low damage (12)",     "Fast attack speed",   "Explosive lower damage pellet"}}},
    {{ELE_WIND, ELE_FIRE, ELE_NONE},     {PROJECTILE_AREA_GUST,           100, 2.f,  0,   TEXTURE_BOX_FIRE_WIND,    {"Wind Fire tower",     "Area of Effect Gust",          "No damage (0)",       "Medium attack speed", "Holds close enough targets in place"}}},
    {{ELE_ELECTRIC, ELE_FIRE, ELE_NONE}, {PROJECTILE_EXPLODING_LIGHTNING, 100, 2.5f, 25,  TEXTURE_BOX_FIRE_ELECTRO, {"Electric Fire tower", "Targeted Exploding Lightning", "Medium damage (25)",  "Slow attack speed",   "Chains explosions between targets"}}},
    {{ELE_EARTH, ELE_WIND, ELE_NONE},    {PROJECTILE_PELLET,              150, 0.3f, 15,  TEXTURE_BOX_EARTH_WIND,   {"Earth Wind tower",    "Targeted Pellet",              "Low damage (15)",     "Fast attack speed",   "Single target damage"}}}, // note: no difference to only earth tower (except numbers)
    {{ELE_EARTH, ELE_WATER, ELE_NONE},   {PROJECTILE_PELLET,              250, 0.8f, 25,  TEXTURE_BOX_EARTH_WATER,  {"Earth Water tower",   "Targeted Pellet",              "Medium damage (25)",  "Fast attack speed",   "Middle ground between water and earth"}}},
    {{ELE_WATER, ELE_WIND, ELE_NONE},    {PROJECTILE_ICE_SHARD,           250, 1.5f, 80,  TEXTURE_BOX_WATER_WIND,   {"Water Wind tower",    "Ice Shard with Gust",          "Medium damage (80)",  "Medium attack speed", "Holds target in place"}}},
    // note: missing electro combinations with other elements

    // triple combinations
    // {{ele1, ele2, ele3},              {projectile_type,     range, cooldown, damage, texture,   {towerName,                 projectileType,                 damage,                attackSpeed,           specialEffect},            {r,g,b,a}}}
    {{ELE_FIRE, ELE_FIRE, ELE_FIRE},     {PROJECTILE_BOMB,      125, 1.5f,  100, TEXTURE_BOX,      {"Fire Fire Fire tower",    "Lobbed Bomb Bigger Area",      "Medium damage (100)", "Medium attack speed", "Improved damage"},        {200, 100, 100, 255} }},
    {{ELE_WATER, ELE_WATER, ELE_WATER},  {PROJECTILE_JET,       250, 2.0f,  300, TEXTURE_BOX_BLUE, {"Water Water Water tower", "Targeted Stronger Jet",        "High damage (300)",   "Medium attack speed", "Improved attack speed"},  {100, 100, 200, 255}}},
    {{ELE_WIND, ELE_WIND, ELE_WIND},     {PROJECTILE_AREA_GUST, 150, 1.25f, 0,   TEXTURE_BOX_AIR,  {"Wind Wind Wind tower",    "Targeted Slower (?) Gust",     "No damage (0)",       "Medium attack speed", "Worse (?) attack speed"}, {125, 125, 125, 255}}},
    {{ELE_WATER, ELE_WATER, ELE_WIND},   {PROJECTILE_JET_BOMB,  250, 1.5f,  100, TEXTURE_BOX,      {"Water Water Wind tower",  "Stronger Ice Shard with Gust", "Medium damage (100)", "Medium attack speed", "Improved damage"},        {200, 200, 200, 200}}},
    // etc...
};

static const int g_num_combinations = sizeof(g_element_combinations) / sizeof(ElementCombo);

void SortDescendingElementsInPlace(ELEMENT* elements)
{
    // bubble sort
    for (int i = 0; i < MAX_ELEMENTS - 1; i++) {
        for (int j = 0; j < MAX_ELEMENTS - 1 - i; j++) {
            if (elements[j] < elements[j + 1]) {
                ELEMENT temp = elements[j];
                elements[j] = elements[j + 1];
                elements[j + 1] = temp;
            }
        }
    }
}

static bool ElementsMatch(const ELEMENT* a, const ELEMENT* b) {
    for (int i = 0; i < MAX_ELEMENTS; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

TowerData* GetTowerDataForElements(ELEMENT* sorted_elements) {
    for (int i = 0; i < g_num_combinations; i++) {
        if (ElementsMatch(sorted_elements, g_element_combinations[i].elements)) {
            return &g_element_combinations[i].tower_data;
        }
    }
    return nullptr; // no match found
}

void ResolveElementSystem(SceneBase* scene)
{
    ForEachComponent<ElementComponent, ResolveElementComponent>(scene, [&](EntityID entity, ElementComponent* elementC, ResolveElementComponent* _)
    {
        SortDescendingElementsInPlace(elementC->elements);

        TowerData* tower_data = GetTowerDataForElements(elementC->elements);
        if (tower_data) {
            // apply the tower configuration
            ProjectileSpawnerComponent::Add(entity, tower_data->projectile_type);
            DamageComponent::Add(entity, tower_data->damage);
            CooldownComponent::Add(entity, tower_data->cooldown);
            TowerComponent::Add(entity,TOWER_NONE, tower_data->range, 0); // TODO: remove the unused tower component data (range should be a component, tower type and CD are unused)
            SpriteComponent* sprite = SpriteComponent::Add(entity, ResourceManager::GetTexture(tower_data->tex));

            sprite->colorMod = tower_data->colorMod;

            printf("configured tower with projectile type %d\n", tower_data->projectile_type);
        } else {
            printf("no tower configuration found for element combination\n");
        }

        // remove the resolve component so this doesn't run again
        RemoveComponent<ResolveElementComponent>(entity);
    });
}
