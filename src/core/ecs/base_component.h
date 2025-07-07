#pragma once

// Base component - no Init()
struct Component {
    virtual void Destroy() = 0;
};
