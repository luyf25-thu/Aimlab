#pragma once

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include "MathUtils.h"

// 后坐力组件：管理开火偏移与匀速回正
class RecoilComponent
{
public:
    // 构造函数：设置默认参数
    RecoilComponent(float kick = 4.0f, float shake = 2.0f, float recoverySeconds = 0.5f,
                    float maxY = 0.0f, float jitterY = 0.0f)
        : kickY(kick), shakeX(shake), recoveryTime(recoverySeconds),
          maxOffsetY(maxY), jitterRangeY(jitterY)
    {
    }

    // 应用一次后坐力
    void applyRecoil()
    {
        const float nextY = targetOffsetY + kickY;
        if (maxOffsetY > 0.0f && nextY > maxOffsetY)
        {
            const float jitter = MathUtils::getRandomFloat(-jitterRangeY, jitterRangeY);
            targetOffsetY = std::max(0.0f, maxOffsetY + jitter);
        }
        else
        {
            targetOffsetY = nextY;
        }
        targetOffsetX += MathUtils::getRandomFloat(-shakeX, shakeX);
        const float instantShakeX = MathUtils::getRandomFloat(-shakeX * 0.7f, shakeX * 0.7f);
        const float instantShakeY = MathUtils::getRandomFloat(-kickY * 0.35f, kickY * 0.35f);
        punchOffsetX += instantShakeX;
        punchOffsetY += instantShakeY;
        isRecovering = false;
    }

    // 更新回正逻辑
    void update(float deltaTime, bool isFiring)
    {
        const float punchDecay = std::min(1.0f, punchDecaySpeed * deltaTime);
        punchOffsetX += (0.0f - punchOffsetX) * punchDecay;
        punchOffsetY += (0.0f - punchOffsetY) * punchDecay;

        if (isFiring)
        {
            isRecovering = false;
            const float t = std::min(1.0f, smoothingSpeed * deltaTime);
            currentOffsetX += (targetOffsetX - currentOffsetX) * t;
            currentOffsetY += (targetOffsetY - currentOffsetY) * t;
            return;
        }

        if (!isRecovering)
        {
            recoveryElapsed = 0.0f;
            recoveryStartX = currentOffsetX;
            recoveryStartY = currentOffsetY;
            targetOffsetX = currentOffsetX;
            targetOffsetY = currentOffsetY;
            isRecovering = true;
        }

        if (recoveryTime <= 0.0f)
        {
            currentOffsetX = 0.0f;
            currentOffsetY = 0.0f;
            return;
        }

        recoveryElapsed += deltaTime;
        const float t = std::min(1.0f, recoveryElapsed / recoveryTime);
        currentOffsetX = recoveryStartX * (1.0f - t);
        currentOffsetY = recoveryStartY * (1.0f - t);
        targetOffsetX = currentOffsetX;
        targetOffsetY = currentOffsetY;
    }

    // 获取当前偏移量
    sf::Vector2f getOffset() const
    {
        return { currentOffsetX + punchOffsetX, -(currentOffsetY + punchOffsetY) };
    }

private:
    float currentOffsetY = 0.0f;
    float currentOffsetX = 0.0f;
    float targetOffsetY = 0.0f;
    float targetOffsetX = 0.0f;
    float kickY = 4.0f;
    float shakeX = 2.0f;
    float recoveryTime = 0.5f;
    float maxOffsetY = 0.0f;
    float jitterRangeY = 0.0f;
    float smoothingSpeed = 14.0f;
    float punchOffsetX = 0.0f;
    float punchOffsetY = 0.0f;
    float punchDecaySpeed = 18.0f;
    bool isRecovering = false;
    float recoveryElapsed = 0.0f;
    float recoveryStartX = 0.0f;
    float recoveryStartY = 0.0f;
};
