#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

// Forward declarations
struct Texture {
    SDL_Texture* sdlTexture;
    int width;
    int height;
};

struct Font {
    TTF_Font* sdlFont;
    int size;
};

struct Sound {
    Mix_Chunk* sdlChunk;
};


// Define sound IDs
enum SoundID
{
    SOUND_NONE = 0,
    SOUND_HIT,
    SOUND_BACKGROUND_MUSIC,
    SOUND_BLIP_HIGH,
    SOUND_BLIP_MID,
    SOUND_BOOM,
    SOUND_BOOM_LOW,
    SOUND_HIT_HARD,
    SOUND_HIT_NOISE,
    SOUND_JUMP,
    SOUND_SHOOT_HIGH,
    SOUND_SHOOT_LOW,
    SOUND_SHOOT_LOW1,
    SOUND_MAX
};

// Define font IDs
enum FontID {
    FONT_NONE = 0,
    FONT_FPS,
    FONT_MAX
};

// Resource definitions
struct SoundResource {
    const char* path;
    SoundID id;
};

struct FontResource {
    const char* path;
    int size;
    FontID id;
};

// Define texture IDs
enum TextureID {
    TEXTURE_NONE = 0,
    TEXTURE_BOX,
    TEXTURE_BOX_BLUE,
    TEXTURE_BOX_EARTH,
    TEXTURE_BOX_AIR,
    TEXTURE_BOX_ELECTRO,
    TEXTURE_BOX_MIX,
    TEXTURE_BOX_ENEMY,
    TEXTURE_BASIC_PROJECTILE,
    TEXTURE_BASIC_PROJECTILE_BROWN,
    TEXTURE_EXPLOSION_1,
    TEXTURE_EXPLOSION_2,
    TEXTURE_EXPLOSION_3,
    TEXTURE_GUST_1,
    TEXTURE_GUST_2,
    TEXTURE_GUST_3,
    TEXTURE_GUST_4,
    // add texture enums here

    TEXTURE_MAX,
};


struct TextureResource {
    const char* path;
    TextureID id;
};

// Global resource definitions
static const TextureResource GAME_TEXTURES[] = {
    {"assets/box.png", TEXTURE_BOX},
    {"assets/box_blue.png", TEXTURE_BOX_BLUE},
    {"assets/box_earth.png", TEXTURE_BOX_EARTH},
    {"assets/box_air.png", TEXTURE_BOX_AIR},
    {"assets/box_electro.png", TEXTURE_BOX_ELECTRO},
    {"assets/box_mix.png", TEXTURE_BOX_MIX},
    {"assets/box_enemy.png", TEXTURE_BOX_ENEMY},
    {"assets/basic_projectile.png", TEXTURE_BASIC_PROJECTILE},
    {"assets/basic_projectile_brown.png", TEXTURE_BASIC_PROJECTILE_BROWN},
    {"assets/explosion_1.png", TEXTURE_EXPLOSION_1},
    {"assets/explosion_2.png", TEXTURE_EXPLOSION_2},
    {"assets/explosion_3.png", TEXTURE_EXPLOSION_3},
    {"assets/sprites/CC/gust_1.png", TEXTURE_GUST_1},
    {"assets/sprites/CC/gust_2.png", TEXTURE_GUST_2},
    {"assets/sprites/CC/gust_3.png", TEXTURE_GUST_3},
    {"assets/sprites/CC/gust_4.png", TEXTURE_GUST_4},
    // Add new textures here
};




static const SoundResource GAME_SOUNDS[] = {
    {"assets/sounds/hit.wav", SOUND_HIT},
    {"assets/sounds/malformed_loop_1.mpeg", SOUND_BACKGROUND_MUSIC},
    {"assets/bfxr_sounds/blip_high.wav",SOUND_BLIP_HIGH},
    {"assets/bfxr_sounds/blip_mid.wav",SOUND_BLIP_MID},
    {"assets/bfxr_sounds/Boom.wav",SOUND_BOOM},
    {"assets/bfxr_sounds/Boom_low.wav",SOUND_BOOM_LOW},
    {"assets/bfxr_sounds/Hit_hard.wav",SOUND_HIT_HARD},
    {"assets/bfxr_sounds/Hit_noise.wav",SOUND_HIT_NOISE},
    {"assets/bfxr_sounds/Jump.wav",SOUND_JUMP},
    {"assets/bfxr_sounds/Shoot_high.wav",SOUND_SHOOT_HIGH},
    {"assets/bfxr_sounds/Shoot_low.wav",SOUND_SHOOT_LOW},
    {"assets/bfxr_sounds/Shoot_low1.wav",SOUND_SHOOT_LOW1}
    // Add new sounds here
};

static const FontResource GAME_FONTS[] = {
    {"assets/fonts/VeniteAdoremusStraight-Yzo6v.ttf", 16, FONT_FPS},
    // Add new fonts here
};

struct ResourceManager {
    static void Cleanup();
    
    // Texture management
    static Texture* LoadTexture(const char* path);
    static void UnloadTexture(Texture* texture);
    static void RenderTexture(Texture* texture, int x, int y);

    // New methods for ID-based resources
    static Texture* LoadTexture(const char* path, TextureID id);
    static Texture* GetTexture(TextureID id);
    static void UnloadTexture(TextureID id);

    // Font management
    static Font* LoadFont(const char* path, int size);
    static void UnloadFont(Font* font);
    static Texture *GetTextTexture(Font *font, const char *text, SDL_Color color);
    static int RenderTextAlignedTopRight(Font* font, const char* text, SDL_Color color, int x, int y);
    static void RenderTextAlignedCenter(Font* font, const char* text, SDL_Color color, int x, int y);

    // Sound management
    static Sound* LoadSound(const char* path);
    static void UnloadSound(Sound* sound);

    // New methods for ID-based resources
    static Sound* LoadSound(const char* path, SoundID id);
    static Sound* GetSound(SoundID id);
    static void UnloadSound(SoundID id);

    static Font* LoadFont(const char* path, int size, FontID id);
    static Font* GetFont(FontID id);
    static void UnloadFont(FontID id);

    // New initialization methods
    static bool InitAllResources();
    static void UnloadAllResources();

    // Music playback
    static void PlayMusic(SoundID id, int loops = -1);
    static void StopMusic();
    static void SetMusicVolume(int volume); // 0-128

    static const int music_channel = -1;

private:
    // Fixed-size arrays for resources
    static Texture* textures[TEXTURE_MAX];
    static Sound* sounds[SOUND_MAX];
    static Font* fonts[FONT_MAX];

    // Helper methods for initialization
    static bool InitTextures();
    static bool InitSounds();
    static bool InitFonts();
    static SDL_Surface *MakeMissingTexture();
}; 