#include "music_system.h"
#include <stdio.h>
#include "../../../game/game.h"
#include "math.h"
#include <algorithm>

void MusicSystem::Init() {
    isMusicPlaying = false;
    wasKeyPressed = false;
    backgroundMusicID = SOUND_BACKGROUND_MUSIC;
    helicopterChannel = -1;
    windChannel = -1;
    isHelicopterPlaying = false;
    isWindPlaying = false;
    
    // PlayMusic();
    printf("MusicSystem initialized\n");
}

void MusicSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Handle music toggle with M key
    if (Input::IsKeyDown(SDL_SCANCODE_M)) {
        if (!wasKeyPressed) {
            ToggleMusic();
            wasKeyPressed = true;
        }
    } else {
        wasKeyPressed = false;
    }

}

void MusicSystem::ToggleMusic() {
    if (isMusicPlaying) {
        StopMusic();
    } else {
        PlayMusic();
    }
}

void MusicSystem::PlayMusic() {
    ResourceManager::PlayMusic(backgroundMusicID, -1);  // -1 for infinite loop
    isMusicPlaying = true;
}

void MusicSystem::StopMusic() {
    ResourceManager::StopMusic();
    isMusicPlaying = false;
}

void MusicSystem::Destroy() {
    StopMusic();
    if (isHelicopterPlaying) {
        Mix_HaltChannel(helicopterChannel);
    }
    if (isWindPlaying) {
        Mix_HaltChannel(windChannel);
    }
    printf("MusicSystem destroyed\n");
} 