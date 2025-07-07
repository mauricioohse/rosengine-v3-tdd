#pragma once

#include <SDL.h>

struct Window {
    SDL_Window* sdlWindow;
    SDL_Renderer* renderer;
    int width;
    int height;
    
    // Initialize window
    bool Init(const char* title, int width, int height);
    
    // Cleanup window
    void Cleanup();
    
    // Clear screen
    void Clear();
    
    // Present rendered frame
    void Present();
};
