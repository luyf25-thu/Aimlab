#pragma once

#include "Target.h"
#include <algorithm>
#include <cstdint>

// 静态靶：停留在原地直到被击中或超时
class StaticTarget : public Target
{
public:
    // 初始化静态靶
    void init(const sf::Vector2f& startPos, float r, float lifeTime,
              int requiredHitsParam, const sf::Color& baseColorParam) override
    {
        position = startPos;
        radius = r;
        lifeTimer = lifeTime;
        persistent = lifeTime < 0.0f;
        isActive = true;
        isHitState = false;
        hitTimer = 0.0f;
        requiredHits = std::max(1, requiredHitsParam);
        currentHits = 0;
        baseColor = baseColorParam;
        shape.setRadius(radius);
        shape.setOrigin({ radius, radius });
        shape.setPosition(position);
        shape.setFillColor(baseColor);
    }

    // 更新静态靶生命周期
    void update(float deltaTime) override
    {
        if (!isActive)
        {
            return;
        }
        if (isHitState)
        {
            hitTimer -= deltaTime;
            if (hitTimer <= 0.0f)
            {
                isActive = false;
            }
            return;
        }

        if (persistent)
        {
            return;
        }

        lifeTimer -= deltaTime;
        if (lifeTimer <= 0.0f)
        {
            isActive = false;
        }
    }

    // 命中时先变绿一段时间再消失
    bool onHit() override
    {
        if (!isActive || isHitState)
        {
            return false;
        }
        ++currentHits;
        if (currentHits < requiredHits)
        {
            const float ratio = static_cast<float>(currentHits) / static_cast<float>(requiredHits);
            const float lighten = 0.6f * ratio;
            const auto toByte = [](float value)
            {
                return static_cast<std::uint8_t>(std::clamp(value, 0.0f, 255.0f));
            };
            const float r = baseColor.r + (255.0f - baseColor.r) * lighten;
            const float g = baseColor.g + (255.0f - baseColor.g) * lighten;
            const float b = baseColor.b + (255.0f - baseColor.b) * lighten;
            shape.setFillColor(sf::Color{ toByte(r), toByte(g), toByte(b) });
            return false;
        }

        isHitState = true;
        hitTimer = hitDisplayDuration;
        shape.setFillColor(sf::Color::Green);
        return true;
    }

private:
    bool persistent = false;
};
