#pragma once

#include <array>
#include <random>
#include <optional>
#include "GameMode.h"
#include "MathUtils.h"

// 简单模式：中央 3x3 位置生成，半径固定且较大
class SimpleMode : public GameMode
{
public:
    // 初始化简单模式参数
    SimpleMode(float spacing = 120.0f, float fixedRadius = 35.0f)
        : gridSpacing(spacing), radius(fixedRadius)
    {
    }

    // 创建一次生成参数
    SpawnInfo createSpawn(const sf::Vector2u& areaSize,
                          const std::vector<sf::Vector2f>& occupiedPositions) override
    {
        const sf::Vector2f center(static_cast<float>(areaSize.x) * 0.5f,
                                  static_cast<float>(areaSize.y) * 0.5f);

        const std::array<float, 3> offsets = { -gridSpacing, 0.0f, gridSpacing };
        std::vector<sf::Vector2f> candidates;
        candidates.reserve(9);
        for (int iy = 0; iy < 3; ++iy)
        {
            for (int ix = 0; ix < 3; ++ix)
            {
                const sf::Vector2f position(center.x + offsets[ix], center.y + offsets[iy]);
                if (!isBlocked(position, occupiedPositions, lastHitPosition))
                {
                    candidates.push_back(position);
                }
            }
        }

        if (candidates.empty())
        {
            for (int iy = 0; iy < 3; ++iy)
            {
                for (int ix = 0; ix < 3; ++ix)
                {
                    candidates.emplace_back(center.x + offsets[ix], center.y + offsets[iy]);
                }
            }
        }

        const int index = getRandomIndex(static_cast<int>(candidates.size()));
        return { candidates[index], radius, -1.0f, 1, sf::Color::Red };
    }

    void onTargetHit(const sf::Vector2f& position) override
    {
        lastHitPosition = position;
    }

    void reset() override
    {
        lastHitPosition.reset();
    }

    // 简单模式保持 3 个靶子
    int getDesiredActiveCount() const override
    {
        return 3;
    }

private:
    int getRandomIndex(int maxExclusive)
    {
        std::uniform_int_distribution<int> dist(0, maxExclusive - 1);
        return dist(getRng());
    }

    bool isBlocked(const sf::Vector2f& position,
                   const std::vector<sf::Vector2f>& occupiedPositions,
                   const std::optional<sf::Vector2f>& lastHit) const
    {
        for (const auto& occupied : occupiedPositions)
        {
            if (MathUtils::distance(position, occupied) < 0.1f)
            {
                return true;
            }
        }
        if (lastHit && MathUtils::distance(position, *lastHit) < 0.1f)
        {
            return true;
        }
        return false;
    }

    std::mt19937& getRng()
    {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }

    float gridSpacing = 120.0f;
    float radius = 35.0f;
    float lifeTime = 2.5f;
    std::optional<sf::Vector2f> lastHitPosition;
};
