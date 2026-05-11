#pragma once

#include "Weapon.h"

// M4A1：全自动射击，后坐力小
class M4A1 : public Weapon
{
public:
    // 初始化 M4A1 参数
    M4A1()
    {
        ammoCapacity = 30;
        currentAmmo = ammoCapacity;
        fireCooldown = 0.09f;
        recoil = RecoilComponent(3.5f, 2.0f, 40.0f);
    }

    // M4A1 射击逻辑
    bool fire() override
    {
        if (!canFire())
        {
            return false;
        }
        commitFire();
        return true;
    }

    // M4A1 支持自动射击
    bool isAutomatic() const override
    {
        return true;
    }
};
