#include "resolve_elements_system.h"
#include "engine.h"


// tower data lookup table
struct TowerData {
    PROJECTILE_TYPE projectile_type;
    int range;
    float cooldown;
    int damage;
    TextureID tex = TEXTURE_BOX_MISSING;

};

// element combination lookup
struct ElementCombo {
    ELEMENT elements[MAX_ELEMENTS]; // sorted elements
    TowerData tower_data;
};

// the element combination dictionary
static ElementCombo g_element_combinations[] = {
    // single elements
    {{ELE_FIRE, ELE_NONE, ELE_NONE}, {PROJECTILE_BOMB, 125, 2.0f, 50, TEXTURE_BOX}},
    {{ELE_WATER, ELE_NONE, ELE_NONE}, {PROJECTILE_JET, 250, 2.0f, 200, TEXTURE_BOX_BLUE}},
    {{ELE_EARTH, ELE_NONE, ELE_NONE}, {PROJECTILE_PELLET, 150, 0.33f, 10, TEXTURE_BOX_EARTH}},
    {{ELE_WIND, ELE_NONE, ELE_NONE}, {PROJECTILE_GUST, 150, 1.5f, 0, TEXTURE_BOX_AIR}},
    {{ELE_ELECTRIC, ELE_NONE, ELE_NONE}, {PROJECTILE_LIGHTNING, 150, 3.0f, 150, TEXTURE_BOX_ELECTRO}},
    
    // dual combinations
    {{ELE_WATER, ELE_FIRE, ELE_NONE}, {PROJECTILE_JET_BOMB, 200, 2.0f, 100, TEXTURE_BOX_FIRE_WATER}},
    {{ELE_EARTH, ELE_FIRE, ELE_NONE}, {PROJECTILE_BOMB, 200, .25f, 10, TEXTURE_BOX_FIRE_EARTH}},
    {{ELE_WIND, ELE_FIRE, ELE_NONE}, {PROJECTILE_AREA_GUST, 100, 2, 0, TEXTURE_BOX_FIRE_WIND}},
    
    // add more combinations here...
    // {{ELE_FIRE, ELE_EARTH, ELE_NONE}, {PROJECTILE_MAGMA, 175, 1.5f, 125}},
    // {{ELE_WATER, ELE_WIND, ELE_NONE}, {PROJECTILE_ICE_SHARD, 200, 1.8f, 90}},
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

            printf("configured tower with projectile type %d\n", tower_data->projectile_type);
        } else {
            printf("no tower configuration found for element combination\n");
        }

        // remove the resolve component so this doesn't run again
        g_Engine.entityManager.RemoveComponentFromEntity(entity, C_ResolveElement);
    } END_FOR_EACH
}