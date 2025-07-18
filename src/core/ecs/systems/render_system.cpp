#include "render_system.h"
#include "../../engine.h"
#include <stdio.h>
#include "grid.h"
#include "math.h"

void RenderSystem::Init() {
    printf("RenderSystem initialized\n");
    cameraX = 0.0f;
    cameraY = 0.0f;
}

static void DrawCircle( int32_t centreX, int32_t centreY, int32_t radius)
{
    SDL_Renderer * renderer = g_Engine.window->renderer;
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   SDL_SetRenderDrawColor(renderer, 122, 122, 122, 122);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}
static void RenderCollider(EntityID entity)
{
    TransformComponent* transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);
    ColliderComponent* collider = (ColliderComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_COLLIDER);
    
    if (!transform || !collider) return;
    
    SDL_Renderer* renderer = g_Engine.window->renderer;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    int x = (int)(transform->x - collider->width / 2);
    int y = (int)(transform->y - collider->height / 2);
    int w = (int)collider->width;
    int h = (int)collider->height;
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

static void RenderJet(EntityID entity)
{
    // draw a  water Jet from src to dest.
    // life time is .5 seconds. draw a watery jet from src to target, with a splash on the target
    // use the lifetime to change the animation. the jet starts thin, increasis width until .250 seconds, at which point the splash starts and the jet ends
    // the splash is the remaining .25 seconds 

    if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_JET | COMPONENT_LIFETIME)) {
        return;
    }
    
    JetAnimationComponent* jet = (JetAnimationComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_JET);
    LifeTimeComponent* lifetime = (LifeTimeComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_LIFETIME);
    SDL_Renderer* renderer = g_Engine.window->renderer;
    
    float progress = (0.2f - lifetime->remaininglifeTime) / 0.2f; // 0.0 to 1.0
    
    if (progress <= 0.5f) {
        // first half: draw expanding jet
        float jetProgress = progress * 2.0f; // 0.0 to 1.0
        int width = (int)(jetProgress * 8.0f) + 1; // 1 to 9 pixels wide
        
        // calculate line points
        int dx = jet->destX - jet->srcX;
        int dy = jet->destY - jet->srcY;
        float length = sqrtf((float)(dx * dx + dy * dy));
        
        if (length > 0) {
            float normalX = -dy / length;
            float normalY = dx / length;
            
            // draw multiple parallel lines for thickness
            SDL_SetRenderDrawColor(renderer, 100, 150, 255, 200);
            for (int i = -width/2; i <= width/2; i++) {
                int startX = jet->srcX + (int)(normalX * i);
                int startY = jet->srcY + (int)(normalY * i);
                int endX = jet->destX + (int)(normalX * i);
                int endY = jet->destY + (int)(normalY * i);
                SDL_RenderDrawLine(renderer, startX, startY, endX, endY);
            }
        }
    } else {
        // second half: draw splash at destination
        float splashProgress = (progress - 0.5f) * 2.0f; // 0.0 to 1.0
        int splashRadius = (int)(splashProgress * 20.0f) + 5; // 5 to 25 pixels
        
        SDL_SetRenderDrawColor(renderer, 100, 150, 255, (int)(255 * (1.0f - splashProgress)));
        
        // draw splash circles
        for (int r = 0; r < splashRadius; r += 3) {
            DrawCircle(jet->destX, jet->destY, r);
        }
    }
}



static void RenderTowerRange(EntityID entity)
{
    if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_TOWER | COMPONENT_TRANSFORM)) {
        return;
    }
    
    TowerComponent* tower = (TowerComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TOWER);
    TransformComponent* transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);

    Point p = Grid::GetNearestGridPointCenter(transform->x,transform->y);

    DrawCircle(p.x, p.y, tower->range);

}

static void RenderEnemyLife(EntityID entity)
{
    if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_ENEMY | COMPONENT_TRANSFORM)) {
        return;
    }
    
    EnemyComponent* enemy = (EnemyComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_ENEMY);
    TransformComponent* transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);
    
    if (!enemy->alive || enemy->maxHealth <= 0) {
        return;
    }
    
    SDL_Renderer* renderer = g_Engine.window->renderer;
    
    // health bar dimensions
    const int BAR_WIDTH = 12;
    const int BAR_HEIGHT = 2;
    int BAR_OFFSET_Y = -8; // default offset above enemy
    
    // adjust offset if sprite component exists
    if (g_Engine.entityManager.HasComponent(entity, COMPONENT_SPRITE)) {
        SpriteComponent* sprite = (SpriteComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_SPRITE);
        if (sprite->texture) {
            BAR_OFFSET_Y = -(sprite->texture->height / 2) + 15; // above sprite bounds
        }
    }
    
    // calculate bar position (centered above enemy)
    int barX = (int)(transform->x - BAR_WIDTH / 2);
    int barY = (int)(transform->y + BAR_OFFSET_Y);
    
    // calculate health percentage
    float healthPercent = (float)enemy->currHealth / (float)enemy->maxHealth;
    int greenWidth = (int)(BAR_WIDTH * healthPercent);
    int redWidth = BAR_WIDTH - greenWidth;
    
    // draw red background (missing health)
    if (redWidth > 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect redRect = {barX + greenWidth, barY, redWidth, BAR_HEIGHT};
        SDL_RenderFillRect(renderer, &redRect);
    }
    
    // draw green foreground (current health)
    if (greenWidth > 0) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect greenRect = {barX, barY, greenWidth, BAR_HEIGHT};
        SDL_RenderFillRect(renderer, &greenRect);
    }
}

static void RenderChain(EntityID entity){
    // renders a purple rectange from the currXY to nextXY
    if (!g_Engine.entityManager.HasComponent(entity, COMPONENT_CHAIN)) {
        return;
    }
    
    ChainLightningComponent* chain = (ChainLightningComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_CHAIN);
    if (!chain) {
        return;
    }
    
    SDL_Renderer* renderer = g_Engine.window->renderer;
    
    // set purple color
    SDL_SetRenderDrawColor(renderer, 128, 0, 255, 255);
    
    // calculate rectangle dimensions
    const int THICKNESS = 4;
    
    // calculate vector from curr to next
    int dx = chain->nextX - chain->currX;
    int dy = chain->nextY - chain->currY;
    
    // calculate length
    float length = sqrt(dx * dx + dy * dy);
    
    if (length > 0) {
        // normalize direction vector
        float nx = dx / length;
        float ny = dy / length;
        
        // perpendicular vector for thickness
        float px = -ny * (THICKNESS / 2);
        float py = nx * (THICKNESS / 2);
        
        // create the four corners of the rotated rectangle
        SDL_Point points[5];
        points[0].x = chain->currX + (int)px;
        points[0].y = chain->currY + (int)py;
        points[1].x = chain->currX - (int)px;
        points[1].y = chain->currY - (int)py;
        points[2].x = chain->nextX - (int)px;
        points[2].y = chain->nextY - (int)py;
        points[3].x = chain->nextX + (int)px;
        points[3].y = chain->nextY + (int)py;
        points[4] = points[0]; // close the polygon
        
        // draw the filled polygon by connecting the lines
        SDL_RenderDrawLines(renderer, points, 5);
        
        // fill the rectangle by drawing horizontal lines between the edges
        // this is a simple scanline fill for the rotated rectangle
        int minY = points[0].y;
        int maxY = points[0].y;
        for (int i = 1; i < 4; i++) {
            if (points[i].y < minY) minY = points[i].y;
            if (points[i].y > maxY) maxY = points[i].y;
        }
        
        for (int y = minY; y <= maxY; y++) {
            int leftX = INT_MAX, rightX = INT_MIN;
            
            // find intersection points with the rectangle edges at this y level
            for (int i = 0; i < 4; i++) {
                int j = (i + 1) % 4;
                int y1 = points[i].y;
                int y2 = points[j].y;
                
                if ((y1 <= y && y <= y2) || (y2 <= y && y <= y1)) {
                    if (y1 != y2) {
                        int x = points[i].x + (points[j].x - points[i].x) * (y - y1) / (y2 - y1);
                        if (x < leftX) leftX = x;
                        if (x > rightX) rightX = x;
                    } else {
                        // horizontal line case
                        if (points[i].x < leftX) leftX = points[i].x;
                        if (points[i].x > rightX) rightX = points[i].x;
                        if (points[j].x < leftX) leftX = points[j].x;
                        if (points[j].x > rightX) rightX = points[j].x;
                    }
                }
            }
            
            if (leftX != INT_MAX && rightX != INT_MIN && leftX <= rightX) {
                SDL_RenderDrawLine(renderer, leftX, y, rightX, y);
            }
        }
    }
}


static void RenderDebugAOE(EntityID e)
{
    if (g_Engine.entityManager.HasComponent(e, COMPONENT_TRANSFORM | COMPONENT_PROJECTILE)) {
        TransformComponent* transform = (TransformComponent*)g_Engine.componentArrays.GetComponentData(e, COMPONENT_TRANSFORM);
        ProjectileComponent* projectile = (ProjectileComponent*)g_Engine.componentArrays.GetComponentData(e, COMPONENT_PROJECTILE);
        
        // draw red circle at transform position with projectile explosion radius
        DrawCircle(transform->x, transform->y, projectile->explosionRadius);
    }
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
    // 4. render TIMEDSPRITEENTITY
    for (EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_TIMEDSPRITE)) {
            RenderTimedSpriteEntity(entity, components, camera, deltaTime);
        }
    }

    // 5. render debug explosion area
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_PROJECTILE))
        {
            RenderDebugAOE(entity);
        }
    }

    // 6. render enemy healthbar
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_ENEMY | COMPONENT_SPRITE))
        {
            RenderEnemyLife(entity);
        }
    }

    // draw tower range - in the future we should add a button (like shift) to only draw all when pressed
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_TOWER))
        {
            RenderTowerRange(entity);
        }
    }

    // handle Jet animation logic + render
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_JET | COMPONENT_LIFETIME))
        {
            RenderJet(entity);
        }
    }

    // handle Chain lighning animation logic + render
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CHAIN))
        {
            RenderChain(entity);
        }
    }

    // DEBUG: draw all colliders rectangle edges in red
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_COLLIDER))
        {
            RenderCollider(entity);
        }
    }
}

void RenderSystem::RenderTimedSpriteEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera, float deltaTime) {
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
    TimedSpriteComponent* timedSprite = 
        (TimedSpriteComponent*)components->GetComponentData(entity, COMPONENT_TIMEDSPRITE);

    if (!transform || !timedSprite) return;

    // increment current time based on delta time
    timedSprite->currTime += deltaTime;

    // calculate current frame based on currTime and animTime
    int currentFrame = 0;
    if (timedSprite->animTime > 0) {
        currentFrame = timedSprite->currTime / timedSprite->animTime;
        
        // handle looping or clamping
        if (timedSprite->loop) {
            currentFrame = currentFrame % timedSprite->maxSprites;
        } else {
            if (currentFrame >= timedSprite->maxSprites) {
                currentFrame = timedSprite->maxSprites - 1;
            }
        }
    }

    // printf("currentframe: %d, currtime: %.2f, animtime: %.2f, deltaTime %.3f\n", 
    //        currentFrame, timedSprite->currTime, timedSprite->animTime);

    // bounds check
    if (currentFrame >= timedSprite->maxSprites || 
        currentFrame < 0 || 
        !timedSprite->sprites[currentFrame]) return;

    Texture* currentTexture = timedSprite->sprites[currentFrame];

    float screenX = transform->x;
    float screenY = transform->y;
    
    if (camera) {
        screenX -= camera->x;
        screenY -= camera->y;
    }

    SDL_Rect destRect = {
        (int)screenX - currentTexture->width*transform->scale/2,
        (int)screenY - currentTexture->height*transform->scale/2,
        currentTexture->width*transform->scale,
        currentTexture->height*transform->scale
    };

    SDL_RenderCopyEx(
        g_Engine.window->renderer,
        currentTexture->sdlTexture,
        NULL,
        &destRect,
        transform->rotation,
        NULL,
        SDL_FLIP_NONE
    );
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