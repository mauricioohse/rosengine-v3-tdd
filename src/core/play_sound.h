#include "resource_manager.h"

namespace PlaySound {
    void PlaySound(SoundID soundID, int channel = -1);
    void StopSound(int channel = -1);
    void SetVolume(int volume);
    void ToggleSound();
}
