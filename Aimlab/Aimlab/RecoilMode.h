#pragma once

#include <array>
#include <random>
#include <optional>
#include "GameMode.h"
#include "MathUtils.h"

// 压枪模式：中央 3x3 位置生成，需要多次命中
class RecoilMode : public GameMode
{
public:
    // 初始化压枪模式参数
    RecoilMode(float spacing = 120.0f, float fixedRadius = 35.0f, int hitsToRemove = 10)
        : gridSpacing(spacing), radius(fixedRadius), requiredHits(hitsToRemove)
    {
    }

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
        return { candidates[index], radius, -1.0f, requiredHits, sf::Color::Red };
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

    float gridSpacing = 120.0f;
    float radius = 35.0f;
    int requiredHits = 10;
    std::optional<sf::Vector2f> lastHitPosition;
};
