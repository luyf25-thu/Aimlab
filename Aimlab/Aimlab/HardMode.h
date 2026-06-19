#pragma once

#include "GridModeBase.h"

// Hard mode: 5x5 grid, smaller targets.
class HardMode : public GridModeBase
{
public:
    HardMode(float spacing = 80.0f, float fixedRadius = 20.0f)
        : GridModeBase(5, spacing, fixedRadius)
    {
    }
};
