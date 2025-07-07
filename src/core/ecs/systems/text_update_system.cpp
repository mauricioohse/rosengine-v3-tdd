#include "text_update_system.h"
#include "../../engine.h"

void TextUpdateSystem::Init() {
    printf("TextUpdateSystem initialized\n");
}

void TextUpdateSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TEXT)) {
            TextComponent* text = 
                (TextComponent*)components->GetComponentData(entity, COMPONENT_TEXT);
            
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