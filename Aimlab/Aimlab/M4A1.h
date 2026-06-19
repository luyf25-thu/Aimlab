#pragma once

#include "Weapon.h"

class M4A1 : public Weapon
{
public:
    M4A1()
        : Weapon({ 30, 0.09f, true, RecoilComponent(5.5f, 4.5f, 0.7f, 70.0f, 3.0f, 24.0f) })
    {
    }
};
