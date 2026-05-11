#pragma once

#include "Target.h"

// 静态靶：停留在原地直到被击中或超时
class StaticTarget : public Target
{
public:
    // 初始化静态靶
    void init(const sf::Vector2f& startPos, float r, float lifeTime) override
    {
        position = startPos;
        radius = r;
        lifeTimer = lifeTime;
        isActive = true;
        isHitState = false;
        hitTimer = 0.0f;
        shape.setRadius(radius);
        shape.setOrigin({ radius, radius });
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Red);
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

        lifeTimer -= deltaTime;
        if (lifeTimer <= 0.0f)
        {
            isActive = false;
        }
    }

    // 命中时先变绿一段时间再消失
    void onHit() override
    {
        if (!isActive || isHitState)
        {
            return;
        }
        isHitState = true;
        hitTimer = hitDisplayDuration;
        shape.setFillColor(sf::Color::Green);
    }
};
