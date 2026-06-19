#pragma once

#include "Weapon.h"

class Usp : public Weapon
{
public:
    Usp()
        : Weapon({ 12, 0.25f, false, RecoilComponent(6.0f, 2.5f, 0.5f) })
    {
    }
};
