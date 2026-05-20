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
    virtual void update(float deltaTime, bool isFiring)
    {
        timeSinceLastFire += deltaTime;
        recoil.update(deltaTime, isFiring);
        if (!infiniteAmmo && !isReloading && currentAmmo == 0)
        {
            reload();
        }
        if (isReloading)
        {
            reloadTimer -= deltaTime;
            if (reloadTimer <= 0.0f)
            {
                currentAmmo = ammoCapacity;
                isReloading = false;
            }
        }
    }

    // 重新装填子弹
    void reload()
    {
        if (isReloading || currentAmmo == ammoCapacity)
        {
            return;
        }
        isReloading = true;
        reloadTimer = reloadDuration;
    }

    // 获取当前子弹数
    int getCurrentAmmo() const
    {
        return currentAmmo;
    }

    // 获取弹匣容量
    int getAmmoCapacity() const
    {
        return ammoCapacity;
    }

    // 是否处于换弹状态
    bool getIsReloading() const
    {
        return isReloading;
    }

    // 设置是否无限子弹
    void setInfiniteAmmo(bool enabled)
    {
        infiniteAmmo = enabled;
    }

    // 获取无限子弹状态
    bool getInfiniteAmmo() const
    {
        return infiniteAmmo;
    }

    // 是否支持按住自动射击
    virtual bool isAutomatic() const = 0;



    // 获取后坐力偏移
    virtual sf::Vector2f getCrosshairOffset() const
    {
        return { 0.0f, 0.0f };
    }

    // 获取弹道偏移
    sf::Vector2f getRecoilOffset() const
    {
        return recoil.getOffset();
    }

protected:
    // 判断是否满足射击条件
    bool canFire() const
    {
        const bool hasAmmo = infiniteAmmo || currentAmmo > 0;
        return !isReloading && hasAmmo && timeSinceLastFire >= fireCooldown;
    }

    // 记录一次射击
    void commitFire()
    {
        if (!infiniteAmmo)
        {
            --currentAmmo;
        }
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
    bool isReloading = false;
    float reloadTimer = 0.0f;
    float reloadDuration = 2.0f;
    bool infiniteAmmo = false;
};
