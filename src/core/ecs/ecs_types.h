#pragma once
#include <SDL.h>
#include "macro_utils.h"
#include <typeindex>

// Common type definitions
typedef uint32_t EntityID;
typedef uint64_t ComponentType;

// Constants
#define MAX_ENTITIES 1000
#define INVALID_ENTITY 0
#define MAX_ELEMENTS 3

template<typename T>
static std::type_index TypeIndex() {
    return std::type_index(typeid(T));
}
