#include "ui_system.h"
#include "../../engine.h"
#include "../../window.h"
#include <stdio.h>
#include "../../play_sound.h"

void UISystem::Init() {
    printf("UISystem initialized\n");
}

void UISystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Get mouse position
    int mouseX, mouseY;
    Input::GetMousePosition(mouseX, mouseY);

    // Update UI states
    for (EntityID entity : entities) {
        if (!HAS_COMPONENT(entity, C_Transform | C_UIBox)) {
            continue;
        }

        UIBoxComponent* box = 
            (UIBoxComponent*)components->GetComponentData(entity, C_UIBox);
        TransformComponent* transform = 
            (TransformComponent*)components->GetComponentData(entity, C_Transform);

        if (!box || !transform) continue;

        // Check for hover state
        box->isHovered = IsPointInBox(mouseX, mouseY, 
                                    transform->x, transform->y, 
                                    box->width, box->height);

        // Update pressed state based on mouse input
        if (box->isHovered) {
            if (Input::IsMouseButtonDown(SDL_BUTTON_LEFT)) {
                box->isPressed = true;
                printf("entity %d was clicked on\n", entity);
                // // Execute callback if it exists
                // if (box->onClick) {
                //     box->onClick();
                // }
            }
        }

        if(box->isPressed && !Input::IsMouseButtonDown(SDL_BUTTON_LEFT) && box->isHovered) {
            printf("entity %d is pressed\n", entity);
            // Execute callback if it exists
            if (box->onClick) {
                box->onClick();
                PlaySound::PlaySound(SOUND_HIT);
            }
        }

        // Reset pressed state when mouse button is released
        if (!Input::IsMouseButtonDown(SDL_BUTTON_LEFT) || !box->isHovered) {
            box->isPressed = false;
        }
    }
}

bool UISystem::IsPointInBox(float pointX, float pointY, float boxX, float boxY, float boxWidth, float boxHeight) {
    return (pointX >= boxX && pointX <= boxX + boxWidth &&
            pointY >= boxY && pointY <= boxY + boxHeight);
}

void UISystem::Destroy() {
    printf("UISystem destroyed\n");
} 