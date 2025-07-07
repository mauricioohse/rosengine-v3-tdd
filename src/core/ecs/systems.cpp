#include "systems.h"
#include "game/game.h"
#include <stdio.h>

void SystemManager::Init() {
    systemCount = 0;
    for (int i = 0; i < MAX_SYSTEMS; i++) {
        systems[i] = nullptr;
    }
}

void SystemManager::RegisterSystem(System* system) {
    if (systemCount >= MAX_SYSTEMS) {
        printf("Warning: Maximum number of systems reached!\n");
        return;
    }

    if (system) {
        systems[systemCount] = system;
        system->Init();
        systemCount++;
    }
}

void SystemManager::UnregisterSystem(System* system) {
    // Find and remove the system
    for (int i = 0; i < systemCount; i++) {
        if (systems[i] == system) {
            // Call destroy on the system
            system->Destroy();
            
            // Shift remaining systems down
            for (int j = i; j < systemCount - 1; j++) {
                systems[j] = systems[j + 1];
            }
            
            systemCount--;
            systems[systemCount] = nullptr;
            return;
        }
    }
}

void SystemManager::UpdateSystems(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {

    for (int i = 0; i < systemCount; i++) {
        if (systems[i]) {

                // Update all systems when game is active
                systems[i]->Update(deltaTime, entities, components);
        }
    }
}

void SystemManager::Destroy() {
    for (int i = 0; i < systemCount; i++) {
        if (systems[i]) {
            systems[i]->Destroy();
            systems[i] = nullptr;
        }
    }
    systemCount = 0;
} 