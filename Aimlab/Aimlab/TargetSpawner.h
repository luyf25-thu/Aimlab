#pragma once

#include "TargetPool.h"
#include "MathUtils.h"

// 靶子生成器：按照节奏生成靶子
class TargetSpawner
{
public:
    // 初始化生成器
    explicit TargetSpawner(TargetPool* pool, float interval = 1.0f)
        : poolRef(pool), spawnInterval(interval)
    {
    }

    // 更新生成计时并尝试生成靶子
    void update(float deltaTime, const sf::Vector2u& areaSize)
    {
        timeSinceLastSpawn += deltaTime;
        if (timeSinceLastSpawn < spawnInterval || !poolRef)
        {
            return;
        }
        timeSinceLastSpawn = 0.0f;

        Target* target = poolRef->acquireTarget();
        if (!target)
        {
            return;
        }

        const float radius = MathUtils::getRandomFloat(15.0f, 35.0f);
        const float x = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.x) - radius);
        const float y = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.y) - radius);
        const float lifeTime = MathUtils::getRandomFloat(1.5f, 3.5f);
        target->init({ x, y }, radius, lifeTime);
    }

private:
    TargetPool* poolRef = nullptr;
    float spawnInterval = 1.0f;
    float timeSinceLastSpawn = 0.0f;
};
