#pragma once
#include "ecs_types.h"
#include "../resource_manager.h"
#include "string.h"
#include "stdio.h"
#include <float.h>
#include "components/background_component.h"
#include "base_component.h"
#include "component_helpers.h"

// Add camera constants
#define CAMERA_FOLLOW_SPEED 15.0f    // How fast camera catches up to target
#define CAMERA_DEADZONE_X 100.0f     // Horizontal deadzone before camera starts moving
#define CAMERA_DEADZONE_Y 100.0f     // Vertical deadzone before camera starts moving

struct TransformComponent : Component<TransformComponent> {
    float x, y;
    float rotation;
    float scale;

    void Init(float posX, float posY, float rot = 0.0f, float scl = 1.0f) {
        x = posX;
        y = posY;
        rotation = rot;
        scale = scl;
    }

    static TransformComponent* Add(EntityID entity, float x, float y, float rotation = 0.0f, float scale = 1.0f) {
        return ComponentManager<TransformComponent>::Add(entity, x, y, rotation, scale);
    }

    void Destroy() override {
        x = 0.0f;
        y = 0.0f;
        rotation = 0.0f;
        scale = 1.0f;
    }
};

struct SpriteComponent : Component<SpriteComponent> {
    Texture* texture;
    int width, height;
    SDL_Rect srcRect;
    bool isVisible;
    SDL_Color colorMod;  // RGB color modulation (255,255,255 = no change)
    float hueShift;      // Hue shift in degrees (0-360)
    

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
        colorMod = {255, 255, 255, 255}; // default: no shift
        hueShift = 0.0f;
    }

    static SpriteComponent* Add(EntityID entity, Texture* tex) {
        return ComponentManager<SpriteComponent>::Add(entity, tex);
    }

    void ChangeTexture(Texture* newTexture) {
        texture = newTexture;
        if (texture) {
            width = texture->width;
            height = texture->height;
            srcRect = {0, 0, width, height};
        }
    }

    void SetHue(float degrees) {
        hueShift = degrees;
        while (hueShift >= 360.0f) hueShift -= 360.0f;
        while (hueShift < 0.0f) hueShift += 360.0f;
        
        // Convert HSV to RGB for color modulation
        float h = hueShift / 60.0f;
        float c = 1.0f;  // Full saturation for pure hue shift
        float x = c * (1.0f - ((int)h % 2 == 0 ? h - (int)h : (int)h + 1 - h));
        
        float r, g, b;
        if (h < 1) { r = c; g = x; b = 0; }
        else if (h < 2) { r = x; g = c; b = 0; }
        else if (h < 3) { r = 0; g = c; b = x; }
        else if (h < 4) { r = 0; g = x; b = c; }
        else if (h < 5) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        
        colorMod.r = (unsigned char)(r * 255);
        colorMod.g = (unsigned char)(g * 255);
        colorMod.b = (unsigned char)(b * 255);
    }

    void Destroy() override {
        // Note: We don't destroy the texture here as it's managed by ResourceManager
        texture = nullptr;
        width = 0;
        height = 0;
        srcRect = {0, 0, 0, 0};
        colorMod = {255, 255, 255, 255};
        hueShift = 0.0f;
    }
};

struct WASDControllerComponent : Component<WASDControllerComponent> {
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

    static WASDControllerComponent* Add(EntityID entity, float speed = 200.0f, bool enabled = true) {
        return ComponentManager<WASDControllerComponent>::Add(entity, speed, enabled);
    }

    void Destroy() override {
        moveSpeed = 0.0f;
        canMove = false;
        moveX = 0;
        moveY = 0;
        moveForce = 0.0f;
    }
};

struct ColliderComponent : Component<ColliderComponent> {
    float radius;
    bool isTrigger;  // If true, detects collision but doesn't prevent movement
    bool isStatic;   // If true, this object won't be moved during collision resolution
    
    void Init(float r, bool staticCollider = false, bool triggerCollider = false) {
        radius = r;
        isStatic = staticCollider;
        isTrigger = triggerCollider;
    }

    static ColliderComponent* Add(EntityID entity, float r, bool staticCollider = false, bool triggerCollider = false) {
        return ComponentManager<ColliderComponent>::Add(entity, r, staticCollider, triggerCollider);
    }
    
    void Destroy() override {
        radius = 0.0f;
        isTrigger = false;
        isStatic = false;
    }
};

struct AnimationComponent : Component<AnimationComponent> {
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

    static AnimationComponent* Add(EntityID entity, Texture* sheet, int frameW, int frameH, int cols, int frames, 
                    float time = 0.1f, bool shouldLoop = true) {
        return ComponentManager<AnimationComponent>::Add(entity, sheet, frameW, frameH, cols, frames, time, shouldLoop);
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

struct GravityComponent : Component<GravityComponent> {
    float velocityY;       // Current vertical velocity
    float gravityScale;    // Multiplier for gravity (1.0 = normal, 0.5 = half gravity, etc.)
    bool isGrounded;       // Is the entity touching the ground?
    
    void Init(float scale = 1.0f) {
        velocityY = 0.0f;
        gravityScale = scale;
        isGrounded = false;
    }

    static GravityComponent* Add(EntityID entity, float scale = 1.0f) {
        return ComponentManager<GravityComponent>::Add(entity, scale);
    }

    void Destroy() override {
        velocityY = 0.0f;
        gravityScale = 1.0f;
        isGrounded = false;
    }
};

struct CameraComponent : Component<CameraComponent> {
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

    static CameraComponent* Add(EntityID entity, float width, float height, EntityID target = 0) {
        return ComponentManager<CameraComponent>::Add(entity, width, height, target);
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

struct TextComponent : Component<TextComponent> {
    Font* font = nullptr;
    char text[200];
    SDL_Color color = {255, 255, 255, 255};
    Texture* texture = nullptr;
    bool isDirty = true;
    TextAlignment alignment = TEXT_CENTER;
    bool visible;

    void Init(Font* _font, const char* _text, TextAlignment _alignment = TEXT_CENTER) {
        font = _font;
        strncpy(text, _text, sizeof(text) - 1);
        text[sizeof(text) - 1] = '\0';  // Ensure null termination
        alignment = _alignment;
        isDirty = true;
        texture = nullptr;
        visible =  true;
        printf("text component initialized: %s\n", text);
    }

    static TextComponent* Add(EntityID entity, Font* _font, const char* _text, TextAlignment _alignment = TEXT_CENTER) {
        return ComponentManager<TextComponent>::Add(entity, _font, _text, _alignment);
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

struct UIBoxComponent : Component<UIBoxComponent> {
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

    static UIBoxComponent* Add(EntityID entity,
                    float w,
                    float h,
                    SDL_Color bgColor = {50, 50, 50, 255},
                    SDL_Color brdColor = {255, 255, 255, 255},
                    float brdWidth = 2.0f,
                    OnClickCallback callback = nullptr) {
        return ComponentManager<UIBoxComponent>::Add(entity, w, h, bgColor, brdColor, brdWidth, callback);
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
    TOWER_MAX,
    TOWER_DEBUG
};

// makes the transform data rotate along the center + radius
struct RotateComponent : Component<RotateComponent>
{
    float x,y; // reference rotation place
    float rot_speed; // rad/s
    float curr_angle;
    float radius; // px

    void Init(float _x, float _y, float _curr_angle, float _rot_speed, float _radius)
    {
        x = _x;
        y = _y;
        curr_angle = _curr_angle;
        rot_speed = _rot_speed;
        radius = _radius;
    }

    static RotateComponent* Add(EntityID entity, float _x, float _y, float _curr_angle, float _rot_speed, float _radius) {
        return ComponentManager<RotateComponent>::Add(entity, _x, _y, _curr_angle, _rot_speed, _radius);
    }
};

#define MAX_GEMS_PER_TOWER 3

struct TowerComponent : Component<TowerComponent> {
    TOWER_TYPE type;
    int range; // radius
    float attackCD; // attack cooldown, measured in seconds
    float currCD;
    int AOEradius;
    EntityID gems[MAX_GEMS_PER_TOWER]; // hold the gems entities. NOTE(mau): I am not sure this is the best design, coupling entities. but it is working so far!

    void Init(TOWER_TYPE t, int r, float CD) {
        type = t;
        range = r;
        attackCD = CD;
        currCD = 0;
        AOEradius = 1;
        memset(&gems,0,sizeof(gems));
        
    }

    static TowerComponent* Add(EntityID entity, TOWER_TYPE t, int r, float CD) {
        return ComponentManager<TowerComponent>::Add(entity, t, r, CD);
    }

    void Destroy()
    {
        type = (TOWER_TYPE) 0 ;
    }
};



// only base elements
enum ELEMENT {
    ELE_NONE,
    ELE_FIRE,
    ELE_WIND,
    ELE_WATER,
    ELE_EARTH,
    ELE_ELECTRIC,
    // fill above and the g_element_names[] below
    ELE_MAX // should always be last
};

static const char* g_element_names[] = {
    "none",
    "fire", 
    "wind",
    "water",
    "earth",
    "electric"
};

inline const char* GetElementName(ELEMENT element) {
    if (element >= ELE_MAX) return "unknown";
    return g_element_names[element];
}

struct ElementComponent : Component<ElementComponent> {
    ELEMENT elements[MAX_ELEMENTS] = {};

    void Init(ELEMENT new_ele)
    {
        memset(elements,0, sizeof(elements));
        elements[0] = new_ele;
    }

    static ElementComponent* Add(EntityID entity, ELEMENT new_ele) {
        return ComponentManager<ElementComponent>::Add(entity, new_ele);
    }
};

// tag component
struct ResolveElementComponent : Component<ResolveElementComponent> {

    void Init()
    {
        // empty
    }

    static ResolveElementComponent* Add(EntityID entity) {
        return ComponentManager<ResolveElementComponent>::Add(entity);
    }
};

struct LifeTimeComponent : Component<LifeTimeComponent> {
    float remaininglifeTime; // in seconds

    void Init(float lifeTime)
    {
        remaininglifeTime = lifeTime;
    }

    static LifeTimeComponent* Add(EntityID entity, float lifeTime) {
        return ComponentManager<LifeTimeComponent>::Add(entity, lifeTime);
    }

    void Destroy() override 
    {
        remaininglifeTime = 0;
    }
};

// tag
struct DamageOnCollisionComponent : Component<DamageOnCollisionComponent> {

    void Init()
    {
        // empty
    }

    static DamageOnCollisionComponent* Add(EntityID entity) {
        return ComponentManager<DamageOnCollisionComponent>::Add(entity);
    }
};

struct JetAnimationComponent : Component<JetAnimationComponent> {
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

    static JetAnimationComponent* Add(EntityID entity, int _srcX, int _srcY, int _destX, int _destY) {
        return ComponentManager<JetAnimationComponent>::Add(entity, _srcX, _srcY, _destX, _destY);
    }

    void Destroy() override 
    {
        srcX = 0;
        srcY = 0;
        destX = 0;
        destY = 0;
    }
};

struct ChainLightningComponent : Component<ChainLightningComponent> {
    EntityID hits[12]; // considering 12 the maximum amount of jumps possible
    int jumps; // how many times the chain in will (how many enemies will be hit)
    // the lighntining will be drawn between the current target and the next target every jump
    int currX;
    int currY;
    int nextX;
    int nextY;
    int damage;
    bool explodes;
    EntityID target;
    int frameDelay;
    int currFrameDelay;
    int hasDealtDamage;

    void Init(int _currX, int _currY, int _nextX, int _nextY, EntityID _target, int _damage, int _jumps, bool _explodes)
    {
        currX = _currX;
        currY = _currY;
        nextX = _nextX;
        nextY = _nextY;
        damage = _damage;
        target = _target;
        jumps = _jumps;
        explodes = _explodes;
        memset(hits, 0, sizeof(hits));
        frameDelay = 5;
        hasDealtDamage = 0;
        currFrameDelay = 0;
    }

    static ChainLightningComponent* Add(EntityID entity, int _currX, int _currY, int _nextX, int _nextY, EntityID _target, int _damage, int _jumps, bool _explodes) {
        return ComponentManager<ChainLightningComponent>::Add(entity, _currX, _currY, _nextX, _nextY, _target, _damage, _jumps, _explodes);
    }

    void Destroy () override 
    {

    }
};

struct CrowdControlComponent : Component<CrowdControlComponent> {
    EntityID target;
    int targetX;
    int targetY;

    void Init(EntityID tgt, int _targetX, int _targetY)
    {
        target = tgt;
        targetX = _targetX; 
        targetY = _targetY;
    }

    static CrowdControlComponent* Add(EntityID entity, EntityID tgt, int _targetX, int _targetY) {
        return ComponentManager<CrowdControlComponent>::Add(entity, tgt, _targetX, _targetY);
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
    PROJECTILE_JET_BOMB,
    PROJECTILE_PELLET,
    PROJECTILE_ICE_SHARD,
    PROJECTILE_EXPLODING_LIGHTNING,
    PROJECTILE_AREA_GUST
};

struct ProjectileSpawnerComponent : Component<ProjectileSpawnerComponent> {
    PROJECTILE_TYPE type;

    void Init(PROJECTILE_TYPE _type)
    {
        type = _type;
    }

    static ProjectileSpawnerComponent* Add(EntityID entity, PROJECTILE_TYPE _type) {
        return ComponentManager<ProjectileSpawnerComponent>::Add(entity, _type);
    }
};

struct AddSlowOnCollisionComponent : Component<AddSlowOnCollisionComponent> {
    float duration;
    float intensity; // from 0 to 1, in %

    void Init(float _duration, float _intensity)
    {
        duration = _duration;
        intensity = _intensity; 
    }

    static AddSlowOnCollisionComponent* Add(EntityID entity, float _duration, float _intensity) {
        return ComponentManager<AddSlowOnCollisionComponent>::Add(entity, _duration, _intensity);
    }
};

struct SlowComponent : Component<SlowComponent> {
    float duration;
    float intensity; // from 0 to 1, in %

    void Init(float _duration, float _intensity)
    {
        duration = _duration;
        intensity = _intensity; 
    }

    static SlowComponent* Add(EntityID entity, float _duration, float _intensity) {
        return ComponentManager<SlowComponent>::Add(entity, _duration, _intensity);
    }
};

struct CooldownComponent : Component<CooldownComponent> {
    float remainingCD;
    float CD;

    void Init(float _CD)
    {
        CD = _CD;
        remainingCD = 0;
    }

    static CooldownComponent* Add(EntityID entity, float _CD) {
        return ComponentManager<CooldownComponent>::Add(entity, _CD);
    }
};

struct RangeComponent : Component<RangeComponent> {
    int range;
    
    void Init()
    {
        // empty
    }

    static RangeComponent* Add(EntityID entity) {
        return ComponentManager<RangeComponent>::Add(entity);
    }
};

struct DamageComponent : Component<DamageComponent> {
    int damage;

    void Init( int _damage) {damage = _damage;}

    static DamageComponent* Add(EntityID entity, int _damage) {
        return ComponentManager<DamageComponent>::Add(entity, _damage);
    }
};

struct ExplodeOnXYComponent : Component<ExplodeOnXYComponent> {
    int x, y, range;

    void Init(int _x, int _y, int _range)
    {
        x = _x;
        y = _y;
        range = _range;
    }

    static ExplodeOnXYComponent* Add(EntityID entity, int _x, int _y, int _range) {
        return ComponentManager<ExplodeOnXYComponent>::Add(entity, _x, _y, _range);
    }
};

struct MoveToXYComponent : Component<MoveToXYComponent> {
    int targetX;
    int targetY;
    int speed;
    int hasTransformComponent; // used to log missing only once

    void Init(int x, int y, int spd) {
        targetX = x;
        targetY = y;
        speed = spd;
    }

    static MoveToXYComponent* Add(EntityID entity, int x, int y, int spd) {
        return ComponentManager<MoveToXYComponent>::Add(entity, x, y, spd);
    }

    void Destroy() override {
        int targetX = 0;
        int targetY = 0;
        int hasTransformComponent = 0; // used to log missing only once
    }
};

struct TimedSpriteComponent : Component<TimedSpriteComponent> {
    Texture* sprites[5];
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

    static TimedSpriteComponent* Add(EntityID entity, float currTime_, float animTime_, int loop_, int maxSprites_) {
        return ComponentManager<TimedSpriteComponent>::Add(entity, currTime_, animTime_, loop_, maxSprites_);
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

// a TAG used to know if the transform + collider is used to destroy the entity 
struct EnemyExitComponent : Component<EnemyExitComponent> {

    void Init()
    {
        // empty
    }

    static EnemyExitComponent* Add(EntityID entity) {
        return ComponentManager<EnemyExitComponent>::Add(entity);
    }
};

struct EnemyDebugComponent : Component<EnemyDebugComponent> {
    ELEMENT element;
    bool debug;

    void Init(ELEMENT _element)
    {
        element = _element;
        debug = 0;
    }

    static EnemyDebugComponent* Add(EntityID entity, ELEMENT _element) {
        return ComponentManager<EnemyDebugComponent>::Add(entity, _element);
    }

    void Destroy() override {}
};

struct EnemyComponent : Component<EnemyComponent> {
    int alive;
    int currHealth;
    int maxHealth;
    int speed;
    int currPathIdx;
    int type; // using int instead of ENEMY_TYPE to avoid forward declaration issues

    void Init(int health, int _speed, int _type) {
        alive = 1;
        currHealth = health;
        maxHealth = health; 
        currPathIdx = 0;
        speed = _speed;
        type = _type;
    }

    static EnemyComponent* Add(EntityID entity, int health, int _speed, int _type) {
        return ComponentManager<EnemyComponent>::Add(entity, health, _speed, _type);
    }

    void Destroy()
    {
        alive = 0;
    }
};

struct EnemySpawnerComponent : Component<EnemySpawnerComponent> {
    float spawnCooldown;
    float currentCooldown;
    int spawnType; // using int instead of ENEMY_TYPE to avoid forward declaration issues
    int currentSpawns;
    
    void Init(float cooldown, int type) {
        spawnCooldown = cooldown;
        currentCooldown = cooldown;
        spawnType = type;
        currentSpawns = 0;
    }

    static EnemySpawnerComponent* Add(EntityID entity, float cooldown, int type) {
        return ComponentManager<EnemySpawnerComponent>::Add(entity, cooldown, type);
    }
    
    void Destroy() override {
        spawnCooldown = 0.0f;
        currentCooldown = 0.0f;
        spawnType = 0; // ENEMY_BASIC_I equivalent
        currentSpawns = 0;
    }
};

// Component initialization functions declarations only
void InitTransform(EntityID entity, float x, float y, float rotation = 0.0f, float scale = 1.0f);
void InitSprite(EntityID entity, Texture* texture);
void InitWASDController(EntityID entity, float moveSpeed = 200.0f, bool canMove = true);
void InitAnimation(EntityID entity, Texture* sheet, int frameW, int frameH, int cols, int frames, 
                   float time = 0.1f, bool shouldLoop = true);
void InitGravity(EntityID entity, float scale = 1.0f);
void InitSquirrel(EntityID entity);
void InitSquirrelPhysics(EntityID entity);
void InitCamera(EntityID entity, float width, float height, EntityID target = 0);


// struct ComponentArrays {
//     // Component data pools
// #define xcomponent( type, id, ...) type##Component type##s[MAX_ENTITIES];
// #include "components/components.def"
// #undef xcomponent
//     WASDControllerComponent wasdControllers[MAX_ENTITIES];
//     AnimationComponent animations[MAX_ENTITIES];
//     GravityComponent gravities[MAX_ENTITIES];
//     CameraComponent cameras[MAX_ENTITIES];
//     BackgroundComponent backgrounds[MAX_ENTITIES];

//     // Core functions
//     void* GetComponentData(EntityID entity, ComponentType type);
//     void RemoveComponent(EntityID entity, ComponentType type);
    
//     // Add this to ComponentArrays struct
//     void Init() {
//         // Zero out all component arrays
//         memset(this, 0, sizeof(ComponentArrays));
        
//         printf("ComponentArrays initialized\n");
//     }
// };

// class IComponentArray {
// public:
//     virtual void* GetData(EntityID entity) = 0;
//     virtual ~IComponentArray() = default;
// };

// template<typename T>
// class ComponentArray : public IComponentArray {
//     std::vector<T> componentsArray;
// public:
//     ComponentArray(size_t size) : componentsArray(size) {}
//     void* GetData(EntityID entity) override {
//         return &componentsArray[entity];
//     }
// };
