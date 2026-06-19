#pragma once

#include <SFML/Audio.hpp>
#include <optional>
#include "RecoilComponent.h"

struct WeaponConfig
{
    int ammoCapacity = 0;
    float fireCooldown = 0.0f;
    bool automatic = false;
    RecoilComponent recoil;
};

// Base class for all weapons. Shared shooting, reload, ammo, and recoil logic
// lives here; concrete weapons provide different configs.
class Weapon
{
public:
    virtual ~Weapon() = default;

    virtual bool fire()
    {
        if (!canFire())
        {
            return false;
        }
        commitFire();
        return true;
    }

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

    void reload()
    {
        if (isReloading || currentAmmo == ammoCapacity)
        {
            return;
        }
        isReloading = true;
        reloadTimer = reloadDuration;
    }

    int getCurrentAmmo() const
    {
        return currentAmmo;
    }

    int getAmmoCapacity() const
    {
        return ammoCapacity;
    }

    bool getIsReloading() const
    {
        return isReloading;
    }

    void setInfiniteAmmo(bool enabled)
    {
        infiniteAmmo = enabled;
    }

    bool getInfiniteAmmo() const
    {
        return infiniteAmmo;
    }

    virtual bool isAutomatic() const
    {
        return automatic;
    }

    sf::Vector2f getRecoilOffset() const
    {
        return recoil.getOffset();
    }

protected:
    Weapon() = default;

    explicit Weapon(const WeaponConfig& config)
        : recoil(config.recoil),
          ammoCapacity(config.ammoCapacity),
          currentAmmo(config.ammoCapacity),
          fireCooldown(config.fireCooldown),
          automatic(config.automatic)
    {
    }

    bool canFire() const
    {
        const bool hasAmmo = infiniteAmmo || currentAmmo > 0;
        return !isReloading && hasAmmo && timeSinceLastFire >= fireCooldown;
    }

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
    bool automatic = false;
};
