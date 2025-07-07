#pragma once
#include "../base_component.h"
#include "../../engine_constants.h"

struct BackgroundComponent : Component {
    float parallaxFactor;  // How much it moves relative to camera (1.0 = full movement)
    int repeatCount;       // How many times to repeat vertically
    
    void Init(float parallax) {
        parallaxFactor = parallax;
        repeatCount = GAME_HEIGHT / WINDOW_HEIGHT;  // Calculate based on game dimensions
    }

    void Destroy() {}
}; 