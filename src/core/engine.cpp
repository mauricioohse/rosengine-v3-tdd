#include "engine.h"
#include "../game/game.h"
#include "window.h"
#include "input.h"
#include <stdio.h>

// Global engine instance
Engine g_Engine;

bool Engine::Init() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL initialization failed! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image initialization failed! SDL_image Error: %s\n", IMG_GetError());
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("SDL_ttf initialization failed! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer initialization failed! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }

    // // Set initial volume (optional)
    Mix_Volume(-1, 32);  // Set to 50% volume

    // Create window
    g_Engine.window = new Window();
    if (!g_Engine.window->Init("RoseEngine", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return false;
    }

    if (!ResourceManager::InitAllResources()) {
        // error is handled inside function call
        return false;
    }

    g_Engine.isRunning = true;
    g_Engine.lastFrameTime = SDL_GetTicks();
    g_Engine.deltaTime = 0.0f;

    // Initialize engine systems
    g_Engine.entityManager.Init();
    g_Engine.systemManager.Init();
    g_Engine.componentArrays.Init();

    return true;
}

#ifdef __EMSCRIPTEN__
static void emscripten_loop() { 
    g_Engine.RunFrame();
}
#endif

void Engine::RunFrame() {

    // Update input state
    Input::Update();

    // Handle events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                g_Engine.isRunning = false;
                break;
                
            case SDL_KEYDOWN:
                Input::keys[event.key.keysym.scancode] = true;
                Input::keysPressed[event.key.keysym.scancode] = true;
                break;
                
            case SDL_KEYUP:
                Input::keys[event.key.keysym.scancode] = false;
                Input::keysReleased[event.key.keysym.scancode] = true;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                Input::mouseButtons[event.button.button] = true;
                Input::mouseButtonsPressed[event.button.button] = true;
                break;
                
            case SDL_MOUSEBUTTONUP:
                Input::mouseButtons[event.button.button] = false;
                Input::mouseButtonsReleased[event.button.button] = true;
                break;
        }
    }

    // Clear screen
    g_Engine.window->Clear();

    // Update and render game
    g_Game.Update(g_Engine.deltaTime);
    g_Game.Render();

    // Present screen
    g_Engine.window->Present();

    // Calculate delta time
    Uint32 currentTime = SDL_GetTicks();
    g_Engine.deltaTime = (currentTime - g_Engine.lastFrameTime) / 1000.0f;
    g_Engine.lastFrameTime = currentTime;

    // Cap framerate
    Uint32 frameTime = SDL_GetTicks() - currentTime;
    if (frameTime < FRAME_TIME) {
        SDL_Delay(FRAME_TIME - frameTime);
    }
}

void Engine::Run() {
#ifdef __EMSCRIPTEN__
    // Set up the loop
    emscripten_set_main_loop(emscripten_loop, 0, 1);

#else
    while (g_Engine.isRunning) {
        g_Engine.RunFrame();
    }
#endif
}

void Engine::Cleanup() {
    ResourceManager::UnloadAllResources();

    if (g_Engine.window) {
        g_Engine.window->Cleanup();
        delete g_Engine.window;
    }


    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
} 