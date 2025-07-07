#include "resource_manager.h"
#include "engine.h"
#include "window.h"
#include <stdio.h>

// Initialize static arrays
Texture* ResourceManager::textures[TEXTURE_MAX] = {nullptr};
Sound* ResourceManager::sounds[SOUND_MAX] = {nullptr};
Font* ResourceManager::fonts[FONT_MAX] = {nullptr};

void ResourceManager::Cleanup() {
    Mix_CloseAudio();
}

SDL_Surface *ResourceManager::MakeMissingTexture()
{
    const int w = 100;
    const int h = 100;
    SDL_Surface *fb = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    SDL_Rect rect = {0, 0, w / 2, h / 2};
    SDL_FillRect(fb, &rect, SDL_MapRGB(fb->format, 128, 0, 128));
    rect.x = w / 2;
    rect.y = 0;
    rect.w = w / 2;
    rect.h = h / 2;
    SDL_FillRect(fb, &rect, SDL_MapRGB(fb->format, 0, 0, 0));
    rect.x = 0;
    rect.y = h / 2;
    rect.w = w / 2;
    rect.h = h / 2;
    SDL_FillRect(fb, &rect, SDL_MapRGB(fb->format, 0, 0, 0));
    rect.x = w / 2;
    rect.y = h / 2;
    rect.w = w / 2;
    rect.h = h / 2;
    SDL_FillRect(fb, &rect, SDL_MapRGB(fb->format, 128, 0, 128));
    return fb;
}

Texture* ResourceManager::LoadTexture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Failed to load texture: %s! Replacing with missing texture.\n", path);
        surface = MakeMissingTexture();
    }
    SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, surface);
    SDL_FreeSurface(surface);
    if (!sdlTexture)
        return nullptr;
    Texture* texture = new Texture();
    texture->sdlTexture = sdlTexture;
    SDL_QueryTexture(sdlTexture, NULL, NULL, &texture->width, &texture->height);
    return texture;
}

Texture* ResourceManager::LoadTexture(const char* path, TextureID id) {
    if (id <= TEXTURE_NONE || id >= TEXTURE_MAX) {
        printf("Load texture Invalid texture ID: %d\n", id);
        return nullptr;
    }
    
    // Unload existing texture if any
    if (textures[id]) {
        UnloadTexture(id);
    }
    
    textures[id] = LoadTexture(path);  // Use existing load function
    return textures[id];
}

Texture* ResourceManager::GetTexture(TextureID id) {
    if (id <= TEXTURE_NONE || id >= TEXTURE_MAX) {
        printf("Invalid texture ID: %d\n", id);
        return nullptr;
    }

    if (!textures[id]){
        printf("texture ID: %d was not initialized!\n", id);
        return nullptr;
    }

    return textures[id];
}

void ResourceManager::UnloadTexture(Texture* texture) {
    if (texture) {
        if (texture->sdlTexture) {
            SDL_DestroyTexture(texture->sdlTexture);
        }
        delete texture;
    }
}

void ResourceManager::UnloadTexture(TextureID id) {
    if (id <= TEXTURE_NONE || id >= TEXTURE_MAX) return;
    if (textures[id]) {
        UnloadTexture(textures[id]);  // Use existing unload function
        textures[id] = nullptr;
    }
}

Font* ResourceManager::LoadFont(const char* path, int size) {
    TTF_Font* sdlFont = TTF_OpenFont(path, size);
    if (!sdlFont) {
        printf("Failed to load font %s! SDL_ttf Error: %s\n", path, TTF_GetError());
        return nullptr;
    }
    
    Font* font = new Font();
    font->sdlFont = sdlFont;
    font->size = size;
    return font;
}

void ResourceManager::UnloadFont(Font* font) {
    if (font) {
        if (font->sdlFont) {
            TTF_CloseFont(font->sdlFont);
        }
        delete font;
    }
}

Sound* ResourceManager::LoadSound(const char* path) {
    Mix_Chunk* sdlChunk = Mix_LoadWAV(path);
    if (!sdlChunk) {
        printf("Failed to load sound %s! SDL_mixer Error: %s\n", path, Mix_GetError());
        return nullptr;
    }
    
    Sound* sound = new Sound();
    sound->sdlChunk = sdlChunk;
    return sound;
}

void ResourceManager::UnloadSound(Sound* sound) {
    if (sound) {
        if (sound->sdlChunk) {
            Mix_FreeChunk(sound->sdlChunk);
        }
        delete sound;
    }
}

void ResourceManager::RenderTexture(Texture* texture, int x, int y) {
    if (!texture || !texture->sdlTexture) return;
    
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = texture->width;
    destRect.h = texture->height;
    
    SDL_RenderCopy(g_Engine.window->renderer, texture->sdlTexture, NULL, &destRect);
}

Texture* ResourceManager::GetTextTexture(Font* font, const char* text, SDL_Color color){
    if (!font || !font->sdlFont) return nullptr;
    
    SDL_Surface* surface = TTF_RenderText_Solid(font->sdlFont, text, color);
    if (!surface) {
        printf("Failed to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return nullptr;
    }
    
    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, surface);
    if (!sdlTexture) {
        printf("Failed to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return nullptr;
    }
    
    Texture* texture = new Texture();
    texture->sdlTexture = sdlTexture;
    texture->width = surface->w;
    texture->height = surface->h;
    
    SDL_FreeSurface(surface);

    return texture;
}

int ResourceManager::RenderTextAlignedTopRight(Font* font, const char* text, SDL_Color color, int x, int y) {
    Texture* texture = ResourceManager::GetTextTexture(font, text, color);

    if (texture) {
        RenderTexture(texture, x - texture->width, y);
        UnloadTexture(texture);
        return true;
    }

    return false;
}


void ResourceManager::RenderTextAlignedCenter(Font* font, const char* text, SDL_Color color, int x, int y) {
    Texture* texture = ResourceManager::GetTextTexture(font, text, color);

    if (texture) {
        // Calculate centered position
        int destX = x - texture->width/2;
        int destY = y - texture->height/2;
        
        RenderTexture(texture, destX, destY);
        UnloadTexture(texture);
    }
}

Sound* ResourceManager::LoadSound(const char* path, SoundID id) {
    if (id <= SOUND_NONE || id >= SOUND_MAX) {
        printf("Invalid sound ID: %d\n", id);
        return nullptr;
    }
    
    // Unload existing sound if any
    if (sounds[id]) {
        UnloadSound(id);
    }
    
    sounds[id] = LoadSound(path);  // Use existing load function
    return sounds[id];
}

Sound* ResourceManager::GetSound(SoundID id) {
    if (id <= SOUND_NONE || id >= SOUND_MAX) {
        printf("Invalid sound ID: %d\n", id);
        return nullptr;
    }
    return sounds[id];
}

void ResourceManager::UnloadSound(SoundID id) {
    if (id <= SOUND_NONE || id >= SOUND_MAX) return;
    if (sounds[id]) {
        UnloadSound(sounds[id]);  // Use existing unload function
        sounds[id] = nullptr;
    }
}

Font* ResourceManager::LoadFont(const char* path, int size, FontID id) {
    if (id <= FONT_NONE || id >= FONT_MAX) {
        printf("Invalid font ID: %d\n", id);
        return nullptr;
    }
    
    // Unload existing font if any
    if (fonts[id]) {
        UnloadFont(id);
    }
    
    fonts[id] = LoadFont(path, size);  // Use existing load function
    return fonts[id];
}

Font* ResourceManager::GetFont(FontID id) {
    if (id <= FONT_NONE || id >= FONT_MAX) {
        printf("Invalid font ID: %d\n", id);
        return nullptr;
    }
    return fonts[id];
}

void ResourceManager::UnloadFont(FontID id) {
    if (id <= FONT_NONE || id >= FONT_MAX) return;
    if (fonts[id]) {
        UnloadFont(fonts[id]);  // Use existing unload function
        fonts[id] = nullptr;
    }
}

bool ResourceManager::InitAllResources() {
    if (!InitTextures()) return false;
    if (!InitSounds()) return false;
    if (!InitFonts()) return false;
    return true;
}

bool ResourceManager::InitTextures() {
    const int textureCount = sizeof(GAME_TEXTURES) / sizeof(GAME_TEXTURES[0]);
    for (int i = 0; i < textureCount; i++) {
        if (!LoadTexture(GAME_TEXTURES[i].path, GAME_TEXTURES[i].id)) {
            printf("Failed to load texture: %s\n", GAME_TEXTURES[i].path);
            return false;
        }
    }
    return true;
}

bool ResourceManager::InitSounds() {
    const int soundCount = sizeof(GAME_SOUNDS) / sizeof(GAME_SOUNDS[0]);
    for (int i = 0; i < soundCount; i++) {
        if (!LoadSound(GAME_SOUNDS[i].path, GAME_SOUNDS[i].id)) {
            printf("Warning: Failed to load sound: %s\n", GAME_SOUNDS[i].path);
        }
    }
    return true;
}

bool ResourceManager::InitFonts() {
    const int fontCount = sizeof(GAME_FONTS) / sizeof(GAME_FONTS[0]);
    for (int i = 0; i < fontCount; i++) {
        if (!LoadFont(GAME_FONTS[i].path, GAME_FONTS[i].size, GAME_FONTS[i].id)) {
            printf("Failed to load font: %s\n", GAME_FONTS[i].path);
            return false;
        }
    }
    return true;
}

void ResourceManager::UnloadAllResources() {
    // Unload all textures
    for (int i = 1; i < TEXTURE_MAX; i++) {
        UnloadTexture((TextureID)i);
    }
    
    // Unload all sounds
    for (int i = 1; i < SOUND_MAX; i++) {
        UnloadSound((SoundID)i);
    }
    
    // Unload all fonts
    for (int i = 1; i < FONT_MAX; i++) {
        UnloadFont((FontID)i);
    }
}

void ResourceManager::PlayMusic(SoundID id, int loops) {
    Sound* sound = GetSound(id);
    if (!sound || !sound->sdlChunk) {
        printf("Failed to play music: invalid sound ID or not loaded\n");
        return;
    }

    // Stop any currently playing music
    StopMusic();

    // Play the new music
    sound->sdlChunk->volume = 16;
    int channel =  Mix_PlayChannel(music_channel, sound->sdlChunk, loops);
    if (channel == -1) {
        printf("Failed to play music! SDL_mixer Error: %s\n", Mix_GetError());
    }

}

void ResourceManager::StopMusic() {
    // Stop all channels (we could be more specific if needed)
    Mix_HaltChannel(music_channel);
}


