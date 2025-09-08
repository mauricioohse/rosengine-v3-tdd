#include "resolve_elements_system.h"
#include "engine.h"
#include "game/tower_placement.h"
#include "systems.h"
#include "scene_manager.h"
#include "game/scenes/main_game_scene.h"

#define DEFAULT_COLORMOD SDL_Color{255,255,255,255}

// the element combination dictionary
static ElementCombo g_element_combinations[] = {
    // single elements - balanced for ~50 dps single target, or 50 dps considering area damage +three targets
    {{ELE_FIRE, ELE_NONE, ELE_NONE}, TowerData{PROJECTILE_BOMB, 125, 2.f, 40, TEXTURE_BOX, DEFAULT_COLORMOD, 75}},
    {{ELE_WATER, ELE_NONE, ELE_NONE}, {PROJECTILE_JET, 250, 3.0f, 150, TEXTURE_BOX_BLUE}},
    {{ELE_EARTH, ELE_NONE, ELE_NONE}, {PROJECTILE_PELLET, 150, 0.5f, 8, TEXTURE_BOX_EARTH}},
    {{ELE_WIND, ELE_NONE, ELE_NONE}, {PROJECTILE_GUST, 150, 2.0f, 0, TEXTURE_BOX_AIR}},
    {{ELE_ELECTRIC, ELE_NONE, ELE_NONE}, {PROJECTILE_LIGHTNING, 150, 2.5f, 60, TEXTURE_BOX_ELECTRO}},
    
    // dual combinations - stronger but more expensive
    {{ELE_FIRE, ELE_FIRE, ELE_NONE}, {PROJECTILE_BOMB, 125, 1.5f, 60, TEXTURE_BOX, {125, 0, 0, 255} , 150}},
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
    {{ELE_FIRE, ELE_FIRE, ELE_FIRE}, {PROJECTILE_BOMB, 125, 1.5f, 90, TEXTURE_BOX, {200, 100, 100, 255} , 300 }},
    {{ELE_WATER, ELE_WATER, ELE_WATER}, {PROJECTILE_JET, 250, 2.0f, 300, TEXTURE_BOX_BLUE, {100, 100, 200, 255}}},
    {{ELE_WIND, ELE_WIND, ELE_WIND}, {PROJECTILE_AREA_GUST, 150, 1.25f, 0, TEXTURE_BOX_AIR, {125,125, 125, 255}}},
    {{ELE_WATER, ELE_WATER, ELE_WIND}, {PROJECTILE_JET_BOMB,250, 1.5f, 100, TEXTURE_BOX, {200,200, 200, 200}}},
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

#include "grid.h"

static EntityID CreateGemEntity(ELEMENT element, EntityID tower, int id)
{
    EntityID e = g_mainGame.RegisterEntity();

    Texture * tex;

    switch (element)
    {
    case ELE_FIRE:
        tex = ResourceManager::GetTexture(TEXTURE_GEM_FIRE);
        break;
    case ELE_ELECTRIC:
        tex = ResourceManager::GetTexture(TEXTURE_GEM_ELECTRO);
        break;
    case ELE_WATER:
        tex = ResourceManager::GetTexture(TEXTURE_GEM_WATER);
        break;
    case ELE_EARTH:
        tex = ResourceManager::GetTexture(TEXTURE_GEM_EARTH);
        break;
    case ELE_WIND:
        tex = ResourceManager::GetTexture(TEXTURE_GEM_WIND);
        break;

    default:
        tex = ResourceManager::GetTexture(TEXTURE_BOX_MISSING);
        break;
    }

    TransformComponent *tower_transform = TransformComponent::Get(tower);

    // initialize the gems with a "RotateComponent" centered at the center of the tower, with rotating radius based on grid square size
    RotateComponent::Add(e, tower_transform->x, tower_transform->y, (2*3.14/3)*id, 5, Grid::GRID_SQUARE_LENGTH / 3); // TODO: CHECK HARDCODED ROTATING SPEED
    TransformComponent::Add(e, tower_transform->x, tower_transform->y, 0, .33); // TODO: check sprite scale
    SpriteComponent::Add(e, tex);
    return e;
}

void ResolveElementSystem(SceneBase * scene)
{
    ForEachComponent<ElementComponent, ResolveElementComponent>(scene, [&](EntityID entity, ElementComponent* elementC, ResolveElementComponent* _)
    {
        SortDescendingElementsInPlace(elementC->elements);

        TowerData* tower_data = GetTowerDataForElements(elementC->elements);
        if (tower_data) {

            // First, check if the tower entity already had any gems. if there were, delete all gems entities before reconstructing it
            TowerComponent *existing_tower_component = TowerComponent::Get(entity);
            
            if (existing_tower_component != nullptr)
            {
                // the entity already was a tower - so we need to delete the gems entities attached to it

                for (int i = 0; i < MAX_GEMS_PER_TOWER; i++)
                {
                    printf("ResolveElementSystem testing sizeof existing_tower_component->gems %d\n", sizeof(existing_tower_component->gems));
                    if (existing_tower_component->gems[i] != INVALID_ENTITY)
                    {
                        g_mainGame.DeleteEntity(existing_tower_component->gems[i]); // TODO: actually, when we get here, the tower component was already resetted. so we need to do this in the tower cleanup before the resolution!
                    }
                }
            }

            // apply the tower configuration
            ProjectileSpawnerComponent::Add(entity, tower_data->projectile_type);
            DamageComponent::Add(entity, tower_data->damage);
            CooldownComponent::Add(entity, tower_data->cooldown);
            TowerComponent* tc = TowerComponent::Add(entity,TOWER_NONE, tower_data->range, 0); // TODO: actually use the tower component more!
            tc->AOEradius = tower_data->AOE_radius;

            SpriteComponent* sprite = SpriteComponent::Add(entity, ResourceManager::GetTexture(tower_data->tex));

            sprite->colorMod = tower_data->colorMod;

            // now, create the gems entities based on the elements
            for (int i = 0; elementC->elements[i] != ELE_NONE && i < MAX_GEMS_PER_TOWER; i++)
            {
                tc->gems[i] = CreateGemEntity(elementC->elements[i], entity, i);
            }

            printf("configured tower with projectile type %d\n", tower_data->projectile_type);
        } else {
            printf("no tower configuration found for element combination\n");
        }

        // remove the resolve component so this doesn't run again
        RemoveComponent<ResolveElementComponent>(entity);
    });
}
