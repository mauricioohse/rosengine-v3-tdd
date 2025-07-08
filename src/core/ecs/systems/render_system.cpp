#include "render_system.h"
#include "../../engine.h"
#include <stdio.h>

void RenderSystem::Init() {
    printf("RenderSystem initialized\n");
    cameraX = 0.0f;
    cameraY = 0.0f;
}

void RenderSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Find the active camera (assuming only one camera for now)
    CameraComponent* camera = nullptr;
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CAMERA)) {
            camera = &components->cameras[entity];
            break;
        }
    }

    // 1. First pass: Render all regular sprite entities
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_SPRITE) &&
            !g_Engine.entityManager.HasComponent(entity, COMPONENT_UIBOX)) {
            RenderSpriteEntity(entity, components, camera);
        }
    }

    // 2. Second pass: Render UI boxes WITHOUT transparency
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_TEXT) &&
            !g_Engine.entityManager.HasComponent(entity, COMPONENT_UIBOX)) {  // Exclude UI elements
            RenderTextEntity(entity, components, camera);
        }
    }

    // 3. Third pass: Render UI boxes with their text
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_UIBOX)) {
            RenderUIEntity(entity, components, camera);
        }
    }
}

void RenderSystem::RenderSpriteEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera) {
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
    SpriteComponent* sprite = 
        (SpriteComponent*)components->GetComponentData(entity, COMPONENT_SPRITE);

    if (!transform || !sprite || !sprite->texture || !sprite->isVisible) return;

    float screenX = transform->x;
    float screenY = transform->y;
    
    if (camera) {
        screenX -= camera->x;
        screenY -= camera->y;
    }

    SDL_Rect destRect = {
        (int)screenX - sprite->width*transform->scale/2,
        (int)screenY - sprite->height*transform->scale/2,
        sprite->width*transform->scale,
        sprite->height*transform->scale
    };

    SDL_RenderCopyEx(
        g_Engine.window->renderer,
        sprite->texture->sdlTexture,
        &sprite->srcRect,
        &destRect,
        transform->rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

void RenderSystem::RenderTextEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera) {
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
    TextComponent* text = 
        (TextComponent*)components->GetComponentData(entity, COMPONENT_TEXT);

    if (!transform || !text || !text->texture) return;

    float screenX = transform->x;
    float screenY = transform->y;
    
    if (camera) {
        screenX -= camera->x;
        screenY -= camera->y;
    }

    // Adjust position based on alignment
    switch (text->alignment) {
        case TEXT_CENTER:
            screenX -= text->texture->width / 2;
            screenY -= text->texture->height / 2;
            break;
        case TEXT_RIGHT:
            screenX -= text->texture->width;
            break;
    }

    SDL_Rect destRect = {
        (int)screenX,
        (int)screenY,
        text->texture->width,
        text->texture->height
    };

    SDL_RenderCopy(
        g_Engine.window->renderer,
        text->texture->sdlTexture,
        NULL,
        &destRect
    );
}

void RenderSystem::RenderUIEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera) {
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
    UIBoxComponent* box = 
        (UIBoxComponent*)components->GetComponentData(entity, COMPONENT_UIBOX);
    TextComponent* text = 
        (TextComponent*)components->GetComponentData(entity, COMPONENT_TEXT);

    if (!transform || !box) return;

    // Adjust colors based on hover/pressed state
    SDL_Color bgColor = box->backgroundColor;
    SDL_Color borderColor = box->borderColor;

    if (box->isPressed) {
        bgColor.r = bgColor.r * 0.7;
        bgColor.g = bgColor.g * 0.7;
        bgColor.b = bgColor.b * 0.7;
    } else if (box->isHovered) {
        bgColor.r = std::min(255, int(bgColor.r * 1.2));
        bgColor.g = std::min(255, int(bgColor.g * 1.2));
        bgColor.b = std::min(255, int(bgColor.b * 1.2));
    }

    // Draw box
    SDL_Rect boxRect = {
        static_cast<int>(transform->x),
        static_cast<int>(transform->y),
        static_cast<int>(box->width),
        static_cast<int>(box->height)
    };
    
    // Draw background
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 
                          bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(g_Engine.window->renderer, &boxRect);

    // Draw border
    if (box->borderWidth > 0) {
        SDL_SetRenderDrawColor(g_Engine.window->renderer, 
                              borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawRect(g_Engine.window->renderer, &boxRect);
    }

    // Draw text if present
    if (text && text->texture) {
        int textX = static_cast<int>(transform->x + (box->width - text->texture->width) / 2);
        int textY = static_cast<int>(transform->y + (box->height - text->texture->height) / 2);

        SDL_Rect textRect = {
            textX,
            textY,
            text->texture->width,
            text->texture->height
        };

        SDL_RenderCopy(
            g_Engine.window->renderer,
            text->texture->sdlTexture,
            NULL,
            &textRect
        );
    }
}


void RenderSystem::RenderAnimatedEntity(TransformComponent* transform, AnimationComponent* anim) {
    if (!anim->spriteSheet || !anim->spriteSheet->sdlTexture) return;

    // Calculate screen position (applying camera offset)
    int screenX = (int)(transform->x - cameraX);
    int screenY = (int)(transform->y - cameraY);
    
    // Create destination rectangle
    SDL_Rect destRect;
    destRect.x = screenX;
    destRect.y = screenY;
    destRect.w = anim->frameWidth * transform->scale;
    destRect.h = anim->frameHeight * transform->scale;
    
    // Calculate rotation center
    SDL_Point center = {
        (int)(anim->frameWidth * transform->scale / 2),
        (int)(anim->frameHeight * transform->scale / 2)
    };
    
    // Render the sprite
    SDL_RenderCopyEx(
        g_Engine.window->renderer,
        anim->spriteSheet->sdlTexture,
        &anim->frameRect,
        &destRect,
        transform->rotation,
        &center,
        SDL_FLIP_NONE
    );
}

void RenderSystem::Destroy() {
    printf("RenderSystem destroyed\n");
} 