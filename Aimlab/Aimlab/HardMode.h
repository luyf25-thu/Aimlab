#pragma once

#include <array>
#include <random>
#include <optional>
#include "GameMode.h"
#include "MathUtils.h"

// 困难模式：中央 5x5 位置生成，半径更小
class HardMode : public GameMode
{
public:
    // 初始化困难模式参数
    HardMode(float spacing = 80.0f, float fixedRadius = 20.0f)
        : gridSpacing(spacing), radius(fixedRadius)
    {
    }

    // 创建一次生成参数
    SpawnInfo createSpawn(const sf::Vector2u& areaSize,
                          const std::vector<sf::Vector2f>& occupiedPositions) override
    {
        const sf::Vector2f center(static_cast<float>(areaSize.x) * 0.5f,
                                  static_cast<float>(areaSize.y) * 0.5f);

        const std::array<float, 5> offsets = {
            -2.0f * gridSpacing,
            -1.0f * gridSpacing,
            0.0f,
            1.0f * gridSpacing,
            2.0f * gridSpacing
        };

        std::vector<sf::Vector2f> candidates;
        candidates.reserve(25);
        for (int iy = 0; iy < 5; ++iy)
        {
            for (int ix = 0; ix < 5; ++ix)
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
            for (int iy = 0; iy < 5; ++iy)
            {
                for (int ix = 0; ix < 5; ++ix)
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

    std::mt19937& getRng()
    {
        static std::mt19937 rng(std::random_device{}());
        return rng;
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

    float gridSpacing = 80.0f;
    float radius = 20.0f;
    std::optional<sf::Vector2f> lastHitPosition;
};
