#include "resolve_elements_system.h"
#include "engine.h"


// tower data lookup table
struct TowerData {
    PROJECTILE_TYPE projectile_type;
    int range;
    float cooldown;
    int damage;
    TextureID tex = TEXTURE_BOX_MISSING;
    SDL_Color colorMod = {255, 255, 255, 255};

};

// element combination lookup
struct ElementCombo {
    ELEMENT elements[MAX_ELEMENTS]; // sorted elements
    TowerData tower_data;
};

// the element combination dictionary
static ElementCombo g_element_combinations[] = {
    // single elements - balanced for ~50 dps single target, or 50 dps considering area damage +three targets
    {{ELE_FIRE, ELE_NONE, ELE_NONE}, {PROJECTILE_BOMB, 125, 2.f, 40, TEXTURE_BOX}},
    {{ELE_WATER, ELE_NONE, ELE_NONE}, {PROJECTILE_JET, 250, 3.0f, 150, TEXTURE_BOX_BLUE}},
    {{ELE_EARTH, ELE_NONE, ELE_NONE}, {PROJECTILE_PELLET, 150, 0.5f, 8, TEXTURE_BOX_EARTH}},
    {{ELE_WIND, ELE_NONE, ELE_NONE}, {PROJECTILE_GUST, 150, 2.0f, 0, TEXTURE_BOX_AIR}},
    {{ELE_ELECTRIC, ELE_NONE, ELE_NONE}, {PROJECTILE_LIGHTNING, 150, 2.5f, 60, TEXTURE_BOX_ELECTRO}},
    
    // dual combinations - stronger but more expensive
    {{ELE_FIRE, ELE_FIRE, ELE_NONE}, {PROJECTILE_BOMB, 125, 1.5f, 60, TEXTURE_BOX, {125, 0, 0, 255} }},
    {{ELE_WATER, ELE_WATER, ELE_NONE}, {PROJECTILE_JET, 250, 3.0f, 300, TEXTURE_BOX_BLUE, {0, 0, 125, 255}}},
    {{ELE_WIND, ELE_WIND, ELE_NONE}, {PROJECTILE_GUST, 150, 1.0f, 0, TEXTURE_BOX_AIR, {125,125, 125, 255}}},
    {{ELE_WATER, ELE_FIRE, ELE_NONE}, {PROJECTILE_JET_BOMB, 150, 3.f, 100, TEXTURE_BOX_FIRE_WATER}},
    {{ELE_EARTH, ELE_FIRE, ELE_NONE}, {PROJECTILE_BOMB, 200, 0.4f, 12, TEXTURE_BOX_FIRE_EARTH}},
    {{ELE_WIND, ELE_FIRE, ELE_NONE}, {PROJECTILE_AREA_GUST, 100, 2.f, 0, TEXTURE_BOX_FIRE_WIND}},
    {{ELE_ELECTRIC, ELE_FIRE, ELE_NONE}, {PROJECTILE_EXPLODING_LIGHTNING, 100, 2.5f, 25, TEXTURE_BOX_FIRE_ELECTRO}},
    {{ELE_EARTH, ELE_WIND, ELE_NONE}, {PROJECTILE_PELLET,150, 0.3f, 15, TEXTURE_BOX_EARTH_WIND}}, 
    {{ELE_EARTH, ELE_WATER, ELE_NONE}, {PROJECTILE_PELLET,250, 0.8f, 25, TEXTURE_BOX_EARTH_WATER}},
    {{ELE_WATER, ELE_WIND, ELE_NONE}, {PROJECTILE_ICE_SHARD,250, 1.5f, 80, TEXTURE_BOX_WATER_WIND}},
    // note: missing electro combinations with other elements

    // triple combinations
    {{ELE_FIRE, ELE_FIRE, ELE_FIRE}, {PROJECTILE_BOMB, 125, 1.5f, 100, TEXTURE_BOX, {200, 100, 100, 255} }},
    {{ELE_WATER, ELE_WATER, ELE_WATER}, {PROJECTILE_JET, 250, 2.0f, 300, TEXTURE_BOX_BLUE, {100, 100, 200, 255}}},
    {{ELE_WIND, ELE_WIND, ELE_WIND}, {PROJECTILE_AREA_GUST, 150, 1.25f, 0, TEXTURE_BOX_AIR, {125,125, 125, 255}}},
    {{ELE_WATER, ELE_WATER, ELE_WIND}, {PROJECTILE_JET_BOMB,250, 1.5f, 100, TEXTURE_BOX, {200,200, 200, 200}}},
    // etc...
};

static const int g_num_combinations = sizeof(g_element_combinations) / sizeof(ElementCombo);

static void SortDescendingElementsInPlace(ELEMENT*elements)
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

static TowerData* GetTowerDataForElements(ELEMENT* sorted_elements) {
    for (int i = 0; i < g_num_combinations; i++) {
        if (ElementsMatch(sorted_elements, g_element_combinations[i].elements)) {
            return &g_element_combinations[i].tower_data;
        }
    }
    return nullptr; // no match found
}

void ResolveElementSystem(SceneBase * scene)
{
    FOR_EACH_COMPONENT_2(scene, entity,
                          Element, elementC,
                          ResolveElement, _)
    {
        SortDescendingElementsInPlace(elementC->elements);

        TowerData* tower_data = GetTowerDataForElements(elementC->elements);
        if (tower_data) {
            // apply the tower configuration
            ADD_ProjectileSpawner(entity, tower_data->projectile_type);
            ADD_Damage(entity, tower_data->damage);
            ADD_Target(entity, 0);
            ADD_Cooldown(entity, tower_data->cooldown);
            ADD_Tower(entity,TOWER_NONE, tower_data->range, 0); // TODO: remove the unused tower component data (range should be a component, tower type and CD are unused)
            ADD_Sprite(entity, ResourceManager::GetTexture(tower_data->tex));

            auto sprite = GET_Sprite(entity);
            sprite->colorMod = tower_data->colorMod;

            printf("configured tower with projectile type %d\n", tower_data->projectile_type);
        } else {
            printf("no tower configuration found for element combination\n");
        }

        // remove the resolve component so this doesn't run again
        g_Engine.entityManager.RemoveComponentFromEntity(entity, C_ResolveElement);
    } END_FOR_EACH
}