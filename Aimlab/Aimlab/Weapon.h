#pragma once

#include <SFML/Audio.hpp>
#include <optional>
#include "RecoilComponent.h"

// 武器抽象基类：定义枪械的公共接口
class Weapon
{
public:
    virtual ~Weapon() = default;

    // 射击接口：子类实现具体逻辑
    virtual bool fire() = 0;

    // 更新 CD 与后坐力
    virtual void update(float deltaTime)
    {
        timeSinceLastFire += deltaTime;
        recoil.update(deltaTime);
    }

    // 重新装填子弹
    void reload()
    {
        currentAmmo = ammoCapacity;
    }

    // 是否支持按住自动射击
    virtual bool isAutomatic() const = 0;

    // 获取后坐力偏移
    sf::Vector2f getRecoilOffset() const
    {
        return recoil.getOffset();
    }

protected:
    // 判断是否满足射击条件
    bool canFire() const
    {
        return timeSinceLastFire >= fireCooldown;
    }

    // 记录一次射击
    void commitFire()
    {
        timeSinceLastFire = 0.0f;
        recoil.applyRecoil();
        if (fireSound)
        {
            fireSound->play();
        }
    }

    RecoilComponent recoil;
    int ammoCapacity = 0;
    int currentAmmo = 0;
    float fireCooldown = 0.0f;
    float timeSinceLastFire = 0.0f;
    std::optional<sf::Sound> fireSound;
};
