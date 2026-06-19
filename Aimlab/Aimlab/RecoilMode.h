#pragma once

#include "GridModeBase.h"

// Recoil mode: 3x3 grid, targets require multiple hits.
class RecoilMode : public GridModeBase
{
public:
    RecoilMode(float spacing = 120.0f, float fixedRadius = 35.0f, int hitsToRemove = 10)
        : GridModeBase(3, spacing, fixedRadius, hitsToRemove)
    {
    }
};
