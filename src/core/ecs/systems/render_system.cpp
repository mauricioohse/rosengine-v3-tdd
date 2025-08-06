#include "render_system.h"
#include "../../engine.h"
#include <stdio.h>
#include "../../game/grid.h"
#include <math.h>
#include "../../core/input.h"
#include <climits>
#include "game/tower_placement.h"
#include "game/scenes/main_game_scene.h"
#include "game/game_systems/resolve_elements_system.h"

// Global variable to track which tower should be hidden during placement preview
static EntityID g_towerToHide = INVALID_ENTITY;

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
    TransformComponent* transform = GET_Transform(entity);
    ColliderComponent* collider = GET_Collider(entity);
    
    if (!transform || !collider) return;
    
    SDL_Renderer* renderer = g_Engine.window->renderer;
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    
    int x = (int)(transform->x - collider->radius / 2);
    int y = (int)(transform->y - collider->radius / 2);
    int w = (int)collider->radius;
    int h = (int)collider->radius;
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}

static void RenderJet(EntityID entity)
{
    // draw a  water Jet from src to dest.
    // life time is .5 seconds. draw a watery jet from src to target, with a splash on the target
    // use the lifetime to change the animation. the jet starts thin, increasis width until .250 seconds, at which point the splash starts and the jet ends
    // the splash is the remaining .25 seconds 

    if (!HAS_COMPONENT(entity, C_JetAnimation | C_LifeTime)) {
        return;
    }

    JetAnimationComponent* jet = GET_JetAnimation(entity);
    LifeTimeComponent* lifetime = GET_LifeTime(entity);
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

static bool ShouldRenderTowerRange(EntityID entity)
{
    if (!HAS_COMPONENT(entity, C_Tower | C_Transform)) {
        return false;
    }

    int mouseX, mouseY;
    Input::GetMousePosition(mouseX, mouseY);
    Point mousePoint = Grid::GetNearestGridPointCenter(mouseX, mouseY);
    TransformComponent* transform = GET_Transform(entity);
    Point towerPoint = Grid::GetNearestGridPointCenter(transform->x, transform->y);

    // if left shift is pressed, render the range
    if (true == Input::IsKeyDown(SDL_SCANCODE_LSHIFT))
    {
        return true;
    }
    // if the mouse is hovering over the tower, render the range
    else if ((mousePoint.x == towerPoint.x) && (mousePoint.y == towerPoint.y) && (!TowerPlacement::isPlacementMode))
    {
        return true;
    }

    return false;
}

static void RenderTowerRange(EntityID entity)
{
    TransformComponent* transform = GET_Transform(entity);
    TowerComponent* tower = GET_Tower(entity);
    Point towerPoint = Grid::GetNearestGridPointCenter(transform->x, transform->y);
    DrawCircle(towerPoint.x, towerPoint.y, tower->range);
}

static void RenderTowerPlacementPreview()
{
    g_towerToHide = INVALID_ENTITY; // reset at start of method

    if (!TowerPlacement::isPlacementMode || ELE_NONE == TowerPlacement::selectedElement) {
        return;
    }
    
    int mouseX, mouseY;
    Input::GetMousePosition(mouseX, mouseY);
    
    if (!Grid::IsInsideGrid(mouseX, mouseY)) {
        return;
    }
    
    Point gridPoint = Grid::GetNearestGridPointCenter(mouseX, mouseY);
    
    // when in placement mode, finds tower in current mouse position
    SceneBase* scene = &g_mainGame;
    bool towerFound = false;
    EntityID existingTower = 0;
    FOR_EACH_COMPONENT_2(scene, tower,
                          Transform, TR,
                          Tower, TC
                          )
    {
        if (TR->x == gridPoint.x && TR->y == gridPoint.y)
        {
            towerFound = true;
            existingTower = tower;
            break;
        }
    } END_FOR_EACH

    TowerData* towerData = 0;
    if (towerFound)
    {
        ElementComponent* existingTowerElement = GET_Element(existingTower);
        if (!existingTowerElement) // existing tower has no Element component, this should never happen
        {
            return;
        }
        
        if (existingTowerElement->elements[2] != ELE_NONE) // the placed tower has 3 elements, no tower will be created
        {
            return;
        }
        else if (existingTowerElement->elements[1] != ELE_NONE) // the placed tower has 2 elements
        {
            if (existingTowerElement->elements[0] != TowerPlacement::selectedElement && existingTowerElement->elements[1] != TowerPlacement::selectedElement) // check if its a unique element to add to current tower
            {
                ELEMENT tempElements[MAX_ELEMENTS] = {existingTowerElement->elements[0], existingTowerElement->elements[1], TowerPlacement::selectedElement};
                SortDescendingElementsInPlace(tempElements);
                towerData = GetTowerDataForElements(tempElements);
                g_towerToHide = existingTower; // hide current tower during preview
            }
        }
        else if (existingTowerElement->elements[0] != ELE_NONE) // the placed tower has 1 element
        {
            if (existingTowerElement->elements[0] != TowerPlacement::selectedElement) // check if its a unique element to add
            {
                ELEMENT tempElements[MAX_ELEMENTS] = {existingTowerElement->elements[0], TowerPlacement::selectedElement, ELE_NONE};
                SortDescendingElementsInPlace(tempElements);
                towerData = GetTowerDataForElements(tempElements);
                g_towerToHide = existingTower; // hide current tower during preview
            }
        }
    }
    else
    {
        // no existing tower found in this grid point
        ELEMENT tempElements[MAX_ELEMENTS] = {TowerPlacement::selectedElement, ELE_NONE, ELE_NONE};
        towerData = GetTowerDataForElements(tempElements);
    }

    // check if we got valid tower data
    if (!towerData) {
        return;
    }

    // draw preview range at mouse position
    DrawCircle(gridPoint.x, gridPoint.y, towerData->range);

    // draw preview tower box at mouse position
    SDL_Rect rect = {gridPoint.x - 24, gridPoint.y - 24, 48, 48};
    
    Texture* texture = ResourceManager::GetTexture(towerData->tex);
    if (!texture || !texture->sdlTexture) { // if invalid texture for tower data sets a fallback sprite, should never happen
        texture = ResourceManager::GetTexture(TEXTURE_BOX_MISSING);
    }
    
    // draw preview tower sprite at mouse position
    SDL_SetTextureAlphaMod(texture->sdlTexture, 64); // 25% transparent new sprite
    SDL_RenderCopy(g_Engine.window->renderer, texture->sdlTexture, NULL, &rect);
    SDL_SetTextureAlphaMod(texture->sdlTexture, 255); // reset transparency
}

static void RenderEnemyLife(EntityID entity)
{
    if (!HAS_COMPONENT(entity, C_Enemy | C_Transform)) {
        return;
    }
    
    EnemyComponent* enemy = GET_Enemy(entity);
    TransformComponent* transform = GET_Transform(entity);
    
    if (!enemy->alive || enemy->maxHealth <= 0) {
        return;
    }
    
    SDL_Renderer* renderer = g_Engine.window->renderer;
    
    // health bar dimensions
    const int BAR_WIDTH = 12;
    const int BAR_HEIGHT = 2;
    int BAR_OFFSET_Y = -8; // default offset above enemy
    
    // adjust offset if sprite component exists
    if (HAS_COMPONENT(entity, C_Sprite)) {
        SpriteComponent* sprite = GET_Sprite(entity);
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
    if (!HAS_COMPONENT(entity, C_ChainLightning)) {
        return;
    }
    
    ChainLightningComponent* chain = GET_ChainLightning(entity);
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
    if (HAS_COMPONENT(e, C_Transform | C_ExplodeOnXY)) {
        TransformComponent* transform = GET_Transform(e);
        
        // draw red circle at transform position with projectile explosion radius 
        DrawCircle(transform->x, transform->y, 100); // TODO: explosion range should not be hardcoded
    }
}

void RenderSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Find the active camera (assuming only one camera for now)
    CameraComponent* camera = nullptr;
    for (EntityID entity : entities) {
        if (HAS_COMPONENT(entity, C_CAMERA)) {
            camera = &components->cameras[entity];
            break;
        }
    }

    // 1. First pass: Render all regular sprite entities
    for (EntityID entity : entities) {
        if (HAS_COMPONENT(entity, C_Transform | C_Sprite) &&
            !HAS_COMPONENT(entity, C_UIBox)) {
            RenderSpriteEntity(entity, components, camera);
        }
    }

    // 2. Second pass: Render UI boxes WITHOUT transparency
    for (EntityID entity : entities) {
        if (HAS_COMPONENT(entity, C_Transform | C_Text) &&
            !HAS_COMPONENT(entity, C_UIBox)) {  // Exclude UI elements
            RenderTextEntity(entity, components, camera);
        }
    }

    // 3. Third pass: Render UI boxes with their text
    for (EntityID entity : entities) {
        if (HAS_COMPONENT(entity, C_Transform | C_UIBox)) {
            RenderUIEntity(entity, components, camera);
        }
    }
    // 4. render TIMEDSPRITEENTITY
    for (EntityID entity : entities) {
        if (HAS_COMPONENT(entity, C_Transform | C_TimedSprite)) {
            RenderTimedSpriteEntity(entity, components, camera, deltaTime);
        }
    }

    // 5. render debug explosion area
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_Transform | C_ExplodeOnXY))
        {
            RenderDebugAOE(entity);
        }
    }

    // 6. render enemy healthbar
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_Transform | C_Enemy | C_Sprite))
        {
            RenderEnemyLife(entity);
        }
    }

    // 7. render tower range with left shift or on mouse hovering over tower
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_Transform | C_Tower))
        {
            if (ShouldRenderTowerRange(entity))
            {
                RenderTowerRange(entity);
            }
        }
    }

    // 8. render tower placement preview range, doesn't depend on an entity
    RenderTowerPlacementPreview();

    // handle Jet animation logic + render
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_JetAnimation | C_LifeTime))
        {
            RenderJet(entity);
        }
    }

    // handle Chain lighning animation logic + render
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_ChainLightning))
        {
            RenderChain(entity);
        }
    }

    // DEBUG: draw all Colliders rectangle edges in red
    for (EntityID entity : entities)
    {
        if (HAS_COMPONENT(entity, C_Collider))
        {
            RenderCollider(entity);
        }
    }
}

void RenderSystem::RenderTimedSpriteEntity(EntityID entity, ComponentArrays* components, CameraComponent* camera, float deltaTime) {
    TransformComponent* transform = 
        (TransformComponent*)components->GetComponentData(entity, C_Transform);
    TimedSpriteComponent* timedSprite = 
        (TimedSpriteComponent*)components->GetComponentData(entity, C_TimedSprite);

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
        (TransformComponent*)components->GetComponentData(entity, C_Transform);
    SpriteComponent* sprite = 
        (SpriteComponent*)components->GetComponentData(entity, C_Sprite);

    if (!transform || !sprite || !sprite->texture || !sprite->isVisible) return;
    
    // Skip rendering if this tower should be hidden during placement preview
    if (entity == g_towerToHide) return;

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
        (TransformComponent*)components->GetComponentData(entity, C_Transform);
    TextComponent* text = 
        (TextComponent*)components->GetComponentData(entity, C_Text);

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
        (TransformComponent*)components->GetComponentData(entity, C_Transform);
    UIBoxComponent* box = 
        (UIBoxComponent*)components->GetComponentData(entity, C_UIBox);
    TextComponent* text = 
        (TextComponent*)components->GetComponentData(entity, C_Text);

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