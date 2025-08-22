#include "systems.h"
#include "scene_manager.h"
#include "engine.h"

struct playerLife_context {
    int health;
    EntityID health_text_entity;
};

void playerLife_init();
void playerLife_decrease_health(int amount);
int playerLife_get_health();