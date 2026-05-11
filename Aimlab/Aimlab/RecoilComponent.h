#pragma once

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "MathUtils.h"

// 后坐力组件：管理开火偏移与匀速回正
class RecoilComponent
{
public:
    // 构造函数：设置默认参数
    RecoilComponent(float kick = 4.0f, float shake = 2.0f, float recovery = 25.0f)
        : kickY(kick), shakeX(shake), recoverySpeed(recovery)
    {
    }

    // 应用一次后坐力
    void applyRecoil()
    {
        currentOffsetY += kickY;
        currentOffsetX += MathUtils::getRandomFloat(-shakeX, shakeX);
    }

    // 更新回正逻辑
    void update(float deltaTime)
    {
        const float recovery = recoverySpeed * deltaTime;
        if (currentOffsetY > 0.0f)
        {
            currentOffsetY = std::max(0.0f, currentOffsetY - recovery);
        }
        if (currentOffsetX > 0.0f)
        {
            currentOffsetX = std::max(0.0f, currentOffsetX - recovery);
        }
        else if (currentOffsetX < 0.0f)
        {
            currentOffsetX = std::min(0.0f, currentOffsetX + recovery);
        }
    }

    // 获取当前偏移量
    sf::Vector2f getOffset() const
    {
        return { currentOffsetX, -currentOffsetY };
    }

private:
    float currentOffsetY = 0.0f;
    float currentOffsetX = 0.0f;
    float kickY = 4.0f;
    float shakeX = 2.0f;
    float recoverySpeed = 25.0f;
};
