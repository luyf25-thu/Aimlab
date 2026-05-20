#pragma once

#include "Weapon.h"

// AK47：全自动射击，后坐力大
class Ak47 : public Weapon
{
public:
    // 初始化 AK47 参数
    Ak47()
    {
        ammoCapacity = 30;
        currentAmmo = ammoCapacity;
        fireCooldown = 0.1f;
        recoil = RecoilComponent(6.5f, 3.5f, 35.0f);
        recoil = RecoilComponent(10.5f, 16.0f, 1.0f, 90.0f, 4.0f);
    }

    // AK47 射击逻辑
    bool fire() override
    {
        if (!canFire())
        {
            return false;
        }
        commitFire();
        return true;
    }

    // AK47 支持自动射击
    bool isAutomatic() const override
    {
        return true;
    }

    // AK47 使用统一后坐力偏移
};
