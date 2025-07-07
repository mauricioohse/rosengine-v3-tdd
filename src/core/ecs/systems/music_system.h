#ifndef MUSIC_SYSTEM_H
#define MUSIC_SYSTEM_H

#include "../systems.h"
#include "../../input.h"
#include "../../resource_manager.h"

struct MusicSystem : System {
    void Init() override;
    void Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) override;
    void Destroy() override;
    
    void ToggleMusic();
    void PlayMusic();
    void StopMusic();
    void UpdateHelicopterSound(EntityID helicopterEntity, EntityID squirrelEntity);
    void UpdateWindSound(EntityID squirrelEntity);

private:
    bool isMusicPlaying;
    bool wasKeyPressed;  // For handling M key toggle
    SoundID backgroundMusicID;
    SoundID helicopterSoundID;
    SoundID windSoundID;
    int helicopterChannel;  // Track the channel used for helicopter sound
    int windChannel;
    bool isHelicopterPlaying;
    bool isWindPlaying;
    const float MAX_HELICOPTER_DISTANCE = 300.0f;  // Distance at which helicopter becomes inaudible
    const float MIN_SPEED_FOR_WIND = 500.0f;  // Minimum speed to start wind sound
    const float MAX_SPEED_FOR_WIND = 1500.0f; // Speed for maximum wind intensity
};

#endif 