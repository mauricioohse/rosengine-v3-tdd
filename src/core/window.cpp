#include "window.h"
#include <stdio.h>

bool Window::Init(const char* title, int width, int height) {
    this->width = width;
    this->height = height;
    
    sdlWindow = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );
    
    if (!sdlWindow) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }
    
    renderer = SDL_CreateRenderer(
        sdlWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

void Window::Clear() {
    SDL_SetRenderDrawColor(renderer, 66, 66, 66, 255);
    SDL_RenderClear(renderer);
}

void Window::Present() {
    SDL_RenderPresent(renderer);
}

void Window::Cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (sdlWindow) {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
    }
}
