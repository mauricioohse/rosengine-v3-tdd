#include "player_life_system.h"
#include "main_game_scene.h"
#include "grid.h"

playerLife_context ctx;

void playerLife_init()
{
    ctx.health = 100;
    // create entity to write life at
    ctx.health_text_entity = g_mainGame.RegisterEntity();
    char text[10];
    snprintf(text, sizeof(text), "%d", ctx.health);
    TextComponent::Add(ctx.health_text_entity, ResourceManager::GetFont(FONT_FPS), text);
    
    Point point = Grid::GRID_END_POINT;
    TransformComponent::Add(ctx.health_text_entity,point.x - 20,point.y + 30,0,1);
}

void playerLife_decrease_health(int amount)
{
    ctx.health -= amount;
    TextComponent* text_c = Get<TextComponent>(ctx.health_text_entity);
    snprintf(text_c->text, sizeof(text_c->text), "%d", ctx.health);
    text_c->isDirty = true;
    printf("health updated to %d\n", ctx.health);

}
