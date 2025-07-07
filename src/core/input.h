#pragma once
#include <SDL.h>

struct Input {
    // Keyboard state
    static bool keys[SDL_NUM_SCANCODES];
    static bool keysPressed[SDL_NUM_SCANCODES];
    static bool keysReleased[SDL_NUM_SCANCODES];
    
    // Mouse state
    static int mouseX;
    static int mouseY;
    static bool mouseButtons[5];
    static bool mouseButtonsPressed[5];
    static bool mouseButtonsReleased[5];
    
    // Initialize input system
    static void Init();
    
    // Update input states
    static void Update();
    
    // Clear per-frame states
    static void Clear();
    
    // Input state checks
    static bool IsKeyDown(SDL_Scancode key);
    static bool IsKeyPressed(SDL_Scancode key);
    static bool IsKeyReleased(SDL_Scancode key);
    
    static bool IsMouseButtonDown(int button);
    static bool IsMouseButtonPressed(int button);
    static bool IsMouseButtonReleased(int button);
    
    static void GetMousePosition(int& x, int& y);
}; 