#pragma once

#include "GridModeBase.h"

// Simple mode: 3x3 grid, larger targets.
class SimpleMode : public GridModeBase
{
public:
    SimpleMode(float spacing = 120.0f, float fixedRadius = 35.0f)
        : GridModeBase(3, spacing, fixedRadius)
    {
    }
};
