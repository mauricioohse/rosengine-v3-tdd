#pragma once

// Base component - no Init()
struct Component {
    virtual void Destroy() {};
    void Init() {};
};
