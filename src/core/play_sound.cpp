#include <SDL.h>
#include "play_sound.h"

bool soundEnabled = true;


void PlaySound::PlaySound(SoundID soundID, int channel) {
    if (!soundEnabled) {
        return;
    }
    Mix_PlayChannel(channel, ResourceManager::GetSound(soundID)->sdlChunk, 0);
}

void PlaySound::StopSound(int channel) {
    Mix_HaltChannel(channel);
}

void PlaySound::SetVolume(int volume) {
    Mix_Volume(-1, volume);
}

void PlaySound::ToggleSound() {
    soundEnabled = !soundEnabled;
}

