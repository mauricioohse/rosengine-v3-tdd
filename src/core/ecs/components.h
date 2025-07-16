#pragma once
#include "ecs_types.h"
#include "../resource_manager.h"
#include "string.h"
#include "stdio.h"
#include "base_component.h"
#include <float.h>
#include "components/background_component.h"


// Add camera constants
#define CAMERA_FOLLOW_SPEED 15.0f     // How fast camera catches up to target
#define CAMERA_DEADZONE_X 100.0f     // Horizontal deadzone before camera starts moving
#define CAMERA_DEADZONE_Y 100.0f     // Vertical deadzone before camera starts moving

struct TransformComponent : Component {
    float x, y;
    float rotation;
    float scale;

    void Init(float posX, float posY, float rot = 0.0f, float scl = 1.0f) {
        x = posX;
        y = posY;
        rotation = rot;
        scale = scl;
    }

    void Destroy() override {
        x = 0.0f;
        y = 0.0f;
        rotation = 0.0f;
        scale = 1.0f;
    }
};

struct SpriteComponent : Component {
    Texture* texture;
    int width, height;
    SDL_Rect srcRect;
    bool isVisible;

    void Init(Texture* tex) {
        texture = tex;
        if (texture) {
            width = texture->width;
            height = texture->height;
            srcRect = {0, 0, width, height};
            isVisible = true;
        } else {
            texture = nullptr;
            width = 0;
            height = 0;
            srcRect = {0, 0, 0, 0};
            isVisible = false;
        }
    }

    void ChangeTexture(Texture* newTexture) {
        texture = newTexture;
        if (texture) {
            width = texture->width;
            height = texture->height;
            srcRect = {0, 0, width, height};
        }
    }

    void Destroy() override {
        // Note: We don't destroy the texture here as it's managed by ResourceManager
        texture = nullptr;
        width = 0;
        height = 0;
        srcRect = {0, 0, 0, 0};
    }
};

struct WASDControllerComponent : Component {
    float moveSpeed;
    bool canMove;
    float moveX;
    float moveY;
    float moveForce;

    void Init(float speed = 200.0f, bool enabled = true) {
        moveSpeed = speed;
        canMove = enabled;
        moveX = 0;
        moveY = 0;
        moveForce = 1.0f;
    }

    void Destroy() override {
        moveSpeed = 0.0f;
        canMove = false;
        moveX = 0;
        moveY = 0;
        moveForce = 0.0f;
    }
};

struct ColliderComponent : Component {
    float width;
    float height;
    bool isTrigger;  // If true, detects collision but doesn't prevent movement
    bool isStatic;   // If true, this object won't be moved during collision resolution
    
    void Init(float w, float h, bool staticCollider = false, bool triggerCollider = false) {
        width = w;
        height = h;
        isStatic = staticCollider;
        isTrigger = triggerCollider;
    }
    
    void Destroy() override {
        width = 0.0f;
        height = 0.0f;
        isTrigger = false;
        isStatic = false;
    }
};

struct AnimationComponent : Component {
    Texture* spriteSheet;            // The sprite sheet texture
    SDL_Rect frameRect;             // Current frame rectangle
    int frameWidth;                 // Width of each frame
    int frameHeight;                // Height of each frame
    int columns;                    // Number of columns in sheet
    int totalFrames;                // Total number of frames
    
    int currentFrame;               // Current frame index
    float frameTime;                // Time per frame (in seconds)
    float accumulator;              // Time accumulator for animation
    bool playing;                   // Is animation playing?
    bool loop;                      // Should animation loop?

    void Init(Texture* sheet, int frameW, int frameH, int cols, int frames, 
              float time = 0.1f, bool shouldLoop = true) {
        spriteSheet = sheet;
        frameWidth = frameW;
        frameHeight = frameH;
        columns = cols;
        totalFrames = frames;
        
        currentFrame = 0;
        frameTime = time;
        accumulator = 0.0f;
        playing = true;
        loop = shouldLoop;

        // Initialize first frame rectangle
        UpdateFrameRect();
    }

    void Destroy() override {
        spriteSheet = nullptr;
        frameRect = {0, 0, 0, 0};
        frameWidth = 0;
        frameHeight = 0;
        columns = 0;
        totalFrames = 0;
        currentFrame = 0;
        frameTime = 0.0f;
        accumulator = 0.0f;
        playing = false;
        loop = false;
    }

    void UpdateFrameRect() {
        frameRect.x = currentFrame % columns * frameWidth;
        frameRect.y = currentFrame / columns * frameHeight;
        frameRect.w = frameWidth;
        frameRect.h = frameHeight;
    }
};

struct GravityComponent : Component {
    float velocityY;       // Current vertical velocity
    float gravityScale;    // Multiplier for gravity (1.0 = normal, 0.5 = half gravity, etc.)
    bool isGrounded;       // Is the entity touching the ground?
    
    void Init(float scale = 1.0f) {
        velocityY = 0.0f;
        gravityScale = scale;
        isGrounded = false;
    }
    
    void Destroy() override {
        velocityY = 0.0f;
        gravityScale = 1.0f;
        isGrounded = false;
    }
};

struct CameraComponent : Component {
    float x, y;              // Camera position (top-left corner)
    float targetX, targetY;  // Position camera is trying to reach
    float viewportWidth;     // Width of the camera view
    float viewportHeight;    // Height of the camera view
    EntityID targetEntity;   // Entity the camera should follow
    float cameraKick;
    
    // Screen shake properties
    float shakeAmount;       // Intensity of the shake
    float shakeTimer;        // Duration of the shake
    
    // Bounds for camera movement
    float minX, maxX;        // Horizontal bounds
    float minY, maxY;        // Vertical bounds
    
    void Init(float width, float height, EntityID target = 0) {
        x = y = 0.0f;
        targetX = targetY = 0.0f;
        viewportWidth = width;
        viewportHeight = height;
        targetEntity = target;
        cameraKick = 0;
        shakeAmount = 0.0f;
        shakeTimer = 0.0f;
        
        // Set reasonable bounds for our game world
        minX = 0.0f;
        maxX = width * 3.0f;
        minY = 0.0f;
        maxY = height * 50.0f;
    }
    
    void Destroy() override {
        x = y = 0.0f;
        targetX = targetY = 0.0f;
        viewportWidth = viewportHeight = 0.0f;
        targetEntity = 0;
        minX = minY = 0.0f;
        maxX = maxY = 0.0f;
        shakeAmount = 0.0f;
        shakeTimer = 0.0f;
    }
};

enum TextAlignment {
    TEXT_CENTER,
    TEXT_RIGHT
};

struct TextComponent : Component {
    Font* font = nullptr;
    char text[200];
    SDL_Color color = {255, 255, 255, 255};
    Texture* texture = nullptr;
    bool isDirty = true;
    TextAlignment alignment = TEXT_CENTER;

    void Init(Font* _font, const char* _text, TextAlignment _alignment = TEXT_CENTER) {
        font = _font;
        strncpy(text, _text, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';  // Ensure null termination
        alignment = _alignment;
        isDirty = true;
        texture = nullptr;
        printf("text component initialized: %s\n", text);
    }

    void Destroy() override {
        font = nullptr;
        text[0] = '\0';
        color = {255, 255, 255, 255};
        texture = nullptr;
        isDirty = true;
        alignment = TEXT_CENTER;
    }
};

struct UIBoxComponent : Component {
    float width;              // Box width
    float height;            // Box height
    SDL_Color backgroundColor;// Background color of the box
    SDL_Color borderColor;   // Border color
    float borderWidth;       // Border thickness
    bool isHovered;          // Is mouse currently over the box?
    bool isPressed;          // Is box currently being pressed?
    
    // Function pointer for click callback
    typedef void (*OnClickCallback)();
    OnClickCallback onClick;
    
    void Init(float w, float h, 
              SDL_Color bgColor = {50, 50, 50, 255},
              SDL_Color brdColor = {255, 255, 255, 255},
              float brdWidth = 2.0f,
              OnClickCallback callback = nullptr) {
        width = w;
        height = h;
        backgroundColor = bgColor;
        borderColor = brdColor;
        borderWidth = brdWidth;
        isHovered = false;
        isPressed = false;
        onClick = callback;
    }
    
    void Destroy() override {
        width = 0.0f;
        height = 0.0f;
        backgroundColor = {0, 0, 0, 0};
        borderColor = {0, 0, 0, 0};
        borderWidth = 0.0f;
        isHovered = false;
        isPressed = false;
        onClick = nullptr;
    }
};

enum TOWER_TYPE{
    TOWER_NONE,
    TOWER_FIRE,
    TOWER_WATER,
    TOWER_FIREWATER,
    TOWER_EARTH,
    TOWER_AIR,
    TOWER_ELECTRIC,
    TOWER_DEBUG
};

struct TowerComponent : Component {
    TOWER_TYPE type;
    int range; // radius
    float attackCD; // attack cooldown, measured in seconds
    float currCD;

    void Init(TOWER_TYPE t, int r, float CD) {
        type = t;
        range = r;
        attackCD = CD;
        currCD=0;
    }

    void Destroy()
    {
        type = (TOWER_TYPE) 0 ;
    }
};

struct LifeTimeComponent : Component {
    float remaininglifeTime; // in seconds

    void Init(float lifeTime)
    {
        remaininglifeTime = lifeTime;
    }

    void Destroy() override 
    {
        remaininglifeTime = 0;
    }
};

struct JetAnimationComponent : Component {
    int srcX;
    int srcY;
    int destX;
    int destY;
    float lifetime;
    int currentStep;

    void Init(int _srcX, int _srcY, int _destX, int _destY)
    {
        srcX = _srcX;
        srcY = _srcY;
        destX = _destX;
        destY = _destY;
        lifetime = .500;
        currentStep = 0;
    }

    void Destroy() override 
    {
        srcX = 0;
        srcY = 0;
        destX = 0;
        destY = 0;
    }
};

struct ChainLightningComponent : Component {
    EntityID hits[12]; // considering 12 the maximum amount of jumps possible
    int jumps; // how many times the chain in will (how many enemies will be hit)
    // the lighntining will be drawn between the current target and the next target every jump
    int currX;
    int currY;
    int nextX;
    int nextY;
    int damage;
    EntityID target;
    int frameDelay;
    int currFrameDelay;
    int hasDealtDamage;

    void Init(int _currX,int _currY,int _nextX,int _nextY, EntityID _target, int _damage, int _jumps )
    {
        currX = _currX;
        currY = _currY;
        nextX = _nextX;
        nextY = _nextY;
        damage = _damage;
        target = _target;
        jumps = _jumps;
        memset(hits, 0, sizeof(hits));
        frameDelay = 5;
        hasDealtDamage = 0;
        currFrameDelay = 0;
    }

    void Destroy () override 
    {

    }
};

struct CrowdcontrolComponent : Component {
    EntityID target;
    int targetX;
    int targetY;

    void Init(EntityID tgt, int _targetX, int _targetY)
    {
        target = tgt;
        targetX = _targetX; 
        targetY = _targetY;
    
    }

    void Destroy() override
    {
        target = 0;
    }
};

enum PROJECTILE_TYPE {
    PROJECTILE_NONE,
    PROJECTILE_BOMB,
    PROJECTILE_JET,
    PROJECTILE_GUST,
    PROJECTILE_LIGHTNING,
    PROJECTILE_PELLET
};

struct ProjectileComponent : Component {

    PROJECTILE_TYPE type;
    int targetX;
    int targetY;
    EntityID targetEntity;
    int shouldExplode;
    int explosionRadius;
    int damage;

    void Init(PROJECTILE_TYPE _type , EntityID target, int _targetX, int _targetY, int dmg, int _shouldExplode, int _explosionRadius){
        damage = dmg;
        targetX = _targetX;
        targetY = _targetY;
        shouldExplode = _shouldExplode;
        explosionRadius = _explosionRadius;
        targetEntity = target;
        type = _type;
    }

    void Destroy() override
    {
        targetX = 0;
        targetY = 0;
        shouldExplode = 0;
        explosionRadius = 0;
    }
};

struct MoveToTargetXYComponent : Component {
    int targetX;
    int targetY;
    int speed;
    int hasTransformComponent; // used to log missing only once

    void Init(int x, int y, int spd) {
        targetX = x;
        targetY = y;
        speed = spd;
    }

    void Destroy() override {
        int targetX = 0;
        int targetY = 0;
        int hasTransformComponent = 0; // used to log missing only once
    }
};

struct TimedSpriteComponent : Component {
    Texture *sprites[5];
    float currTime;
    float animTime; // frame time
    int loop;
    int maxSprites;

    void Init(float currTime_, float animTime_, int loop_, int maxSprites_)
    {
        currTime = currTime_;
        animTime = animTime_;
        loop = loop_;
        maxSprites = maxSprites_;
        for (int i = 0; i < 5; i++) {
            sprites[i] = nullptr;
        }
    }

    void Destroy() override
    {
        currTime = 0.0f;
        animTime = 0.0f;
        loop = 0;
        maxSprites = 0;
        for (int i = 0; i < 5; i++) {
            sprites[i] = nullptr;
        }
    }
};

struct ExplosionComponent : Component {
    int timeToExplode;
};

struct EnemyComponent : Component {
    int alive;
    int currHealth;
    int maxHealth;
    int currPathIdx;

    void Init(int health) {
        alive = 1;
        currHealth = health;
        maxHealth = health; 
        currPathIdx = 0;
    }

    void Destroy()
    {
        alive = 0;
    }

};

// Component initialization functions declarations only
void InitTransform(EntityID entity, float x, float y, float rotation = 0.0f, float scale = 1.0f);
void InitSprite(EntityID entity, Texture* texture);
void InitWASDController(EntityID entity, float moveSpeed = 200.0f, bool canMove = true);
void InitCollider(EntityID entity, float width, float height, bool isStatic = false, bool isTrigger = false);
void InitAnimation(EntityID entity, Texture* sheet, int frameW, int frameH, int cols, int frames, 
                   float time = 0.1f, bool shouldLoop = true);
void InitGravity(EntityID entity, float scale = 1.0f);
void InitSquirrel(EntityID entity);
void InitSquirrelPhysics(EntityID entity);
void InitCamera(EntityID entity, float width, float height, EntityID target = 0);


struct ComponentArrays {
    // Component data pools
#define xcomponent(enum, type, id, ...) type##Component type##s[MAX_ENTITIES];
#include "components/components.def"
#undef xcomponent
    WASDControllerComponent wasdControllers[MAX_ENTITIES];
    ColliderComponent colliders[MAX_ENTITIES];
    AnimationComponent animations[MAX_ENTITIES];
    GravityComponent gravities[MAX_ENTITIES];
    CameraComponent cameras[MAX_ENTITIES];
    BackgroundComponent backgrounds[MAX_ENTITIES];

    // Core functions
    void* GetComponentData(EntityID entity, ComponentType type);
    void RemoveComponent(EntityID entity, ComponentType type);
    
    // Add this to ComponentArrays struct
    void Init() {
        // Zero out all component arrays
        memset(this, 0, sizeof(ComponentArrays));
        
        printf("ComponentArrays initialized\n");
    }
}; 
