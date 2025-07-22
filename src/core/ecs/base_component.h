#pragma once

// Base component - no Init()
struct Component {
    virtual void Destroy() {};
    virtual void Init() {};
};
