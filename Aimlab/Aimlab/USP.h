#pragma once

#include "Weapon.h"

// USP：半自动射击
class Usp : public Weapon
{
public:
    // 初始化 USP 参数
    Usp()
    {
        ammoCapacity = 12;
        currentAmmo = ammoCapacity;
        fireCooldown = 0.25f;
        recoil = RecoilComponent(6.0f, 2.5f, 0.5f); // 保持后坐力逻辑不变
    }

    // USP 射击逻辑
    bool fire() override
    {
        if (!canFire())
        {
            return false;
        }
        commitFire();
        return true;
    }

    // USP 不支持自动射击
    bool isAutomatic() const override
    {
        return false;
    }

    // USP 使用统一后坐力偏移
};
