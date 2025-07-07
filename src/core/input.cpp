#include "input.h"
#include <cstring>
#include "stdio.h"

// Static member initialization
bool Input::keys[SDL_NUM_SCANCODES] = {false};
bool Input::keysPressed[SDL_NUM_SCANCODES] = {false};
bool Input::keysReleased[SDL_NUM_SCANCODES] = {false};

int Input::mouseX = 0;
int Input::mouseY = 0;
bool Input::mouseButtons[5] = {false};
bool Input::mouseButtonsPressed[5] = {false};
bool Input::mouseButtonsReleased[5] = {false};

void Input::Init() {
    // Initialize all arrays to false
    memset(keys, 0, sizeof(keys));
    memset(keysPressed, 0, sizeof(keysPressed));
    memset(keysReleased, 0, sizeof(keysReleased));
    memset(mouseButtons, 0, sizeof(mouseButtons));
    memset(mouseButtonsPressed, 0, sizeof(mouseButtonsPressed));
    memset(mouseButtonsReleased, 0, sizeof(mouseButtonsReleased));
}

void Input::Update() {
    // Clear per-frame states
    memset(keysPressed, 0, sizeof(keysPressed));
    memset(keysReleased, 0, sizeof(keysReleased));
    memset(mouseButtonsPressed, 0, sizeof(mouseButtonsPressed));
    memset(mouseButtonsReleased, 0, sizeof(mouseButtonsReleased));
    
    // Get mouse position
    SDL_GetMouseState(&mouseX, &mouseY);
}

void Input::Clear() {
    memset(keys, 0, sizeof(keys));
    memset(mouseButtons, 0, sizeof(mouseButtons));
}

bool Input::IsKeyDown(SDL_Scancode key) {
    return keys[key];
}

bool Input::IsKeyPressed(SDL_Scancode key) {
    return keysPressed[key];
}

bool Input::IsKeyReleased(SDL_Scancode key) {
    return keysReleased[key];
}

bool Input::IsMouseButtonDown(int button) {
    return mouseButtons[button];
}

bool Input::IsMouseButtonPressed(int button) {
    return mouseButtonsPressed[button];
}

bool Input::IsMouseButtonReleased(int button) {
    return mouseButtonsReleased[button];
}

void Input::GetMousePosition(int& x, int& y) {
    x = mouseX;
    y = mouseY;
} 