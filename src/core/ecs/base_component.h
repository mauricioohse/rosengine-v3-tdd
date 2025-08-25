#pragma once

#include <SDL.h>

typedef uint32_t EntityID;

// Base component - no Init()
struct BaseComponent {
    virtual void Destroy() {};
    void Init() {};
};

template<typename T>
struct Component : BaseComponent {
    static bool RegisterType();
    static T* Get(EntityID entity);

    static bool registered;
};

#include "base_component.tpp"