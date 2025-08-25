#include "text_update_system.h"
#include "../../engine.h"
#include "../components.h"

void TextUpdateSystem::Init() {
    printf("TextUpdateSystem initialized\n");
}

void TextUpdateSystem::Update(float deltaTime, std::vector<EntityID> entities) {
    for (EntityID entity : entities) {
        if (HasComponent<TextComponent>(entity)) {
            TextComponent* text = Get<TextComponent>(entity);

            if (text->isDirty) {
                // Clean up old texture if it exists
                if (text->texture) {
                    ResourceManager::UnloadTexture(text->texture);
                }
                
                // Generate new texture
                text->texture = ResourceManager::GetTextTexture(
                    text->font,
                    text->text,
                    text->color
                );
                
                text->isDirty = false;
            }
        }
    }
}

void TextUpdateSystem::Destroy() {
    printf("TextUpdateSystem destroyed\n");
} 