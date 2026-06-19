#pragma once

#include "Weapon.h"

class Ak47 : public Weapon
{
public:
    Ak47()
        : Weapon({ 30, 0.1f, true, RecoilComponent(10.5f, 16.0f, 1.0f, 135.0f, 4.0f, 48.0f) })
    {
    }
};
