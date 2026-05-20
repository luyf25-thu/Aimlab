#pragma once

#include "TargetPool.h"
#include "MathUtils.h"
#include "GameMode.h"

// 靶子生成器：按照节奏生成靶子
class TargetSpawner
{
public:
    // 初始化生成器
    explicit TargetSpawner(TargetPool* pool, GameMode* mode, float interval = 1.0f)
        : poolRef(pool), modeRef(mode), spawnInterval(interval)
    {
    }

    // 设置当前游戏模式
    void setMode(GameMode* mode)
    {
        modeRef = mode;
    }

    // 更新生成计时并尝试生成靶子
    void update(float deltaTime, const sf::Vector2u& areaSize)
    {
        timeSinceLastSpawn += deltaTime;
        if (!poolRef)
        {
            return;
        }
        const int desiredCount = modeRef ? modeRef->getDesiredActiveCount() : 1;
        int activeCount = countActiveTargets();
        std::vector<sf::Vector2f> occupiedPositions = getActivePositions();

        if (spawnInterval > 0.0f && timeSinceLastSpawn < spawnInterval)
        {
            return;
        }

        timeSinceLastSpawn = 0.0f;
        while (activeCount < desiredCount)
        {
            Target* target = poolRef->acquireTarget();
            if (!target)
            {
                return;
            }

            if (modeRef)
            {
                const SpawnInfo info = modeRef->createSpawn(areaSize, occupiedPositions);
                target->init(info.position, info.radius, info.lifeTime, info.requiredHits, info.baseColor);
                occupiedPositions.push_back(info.position);
            }
            else
            {
                const float radius = MathUtils::getRandomFloat(15.0f, 35.0f);
                const float x = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.x) - radius);
                const float y = MathUtils::getRandomFloat(radius, static_cast<float>(areaSize.y) - radius);
                const float lifeTime = MathUtils::getRandomFloat(1.5f, 3.5f);
                target->init({ x, y }, radius, lifeTime, 1, sf::Color::Red);
            }

            ++activeCount;
            if (spawnInterval > 0.0f)
            {
                break;
            }
        }
    }

private:
    int countActiveTargets() const
    {
        int count = 0;
        for (const auto& target : poolRef->getTargets())
        {
            if (target->getIsActive())
            {
                ++count;
            }
        }
        return count;
    }

    std::vector<sf::Vector2f> getActivePositions() const
    {
        std::vector<sf::Vector2f> positions;
        for (const auto& target : poolRef->getTargets())
        {
            if (target->getIsActive())
            {
                positions.push_back(target->getPosition());
            }
        }
        return positions;
    }

    TargetPool* poolRef = nullptr;
    GameMode* modeRef = nullptr;
    float spawnInterval = 1.0f;
    float timeSinceLastSpawn = 0.0f;
};
