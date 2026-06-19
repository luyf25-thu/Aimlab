#pragma once

#include <optional>
#include <random>
#include <vector>
#include "GameMode.h"
#include "MathUtils.h"

// Grid-based mode base class: handles common spawn-position selection.
class GridModeBase : public GameMode
{
public:
    GridModeBase(int gridSizeParam, float spacingParam, float radiusParam,
                 int requiredHitsParam = 1, int desiredActiveCountParam = 3,
                 float lifeTimeParam = -1.0f, const sf::Color& colorParam = sf::Color::Red)
        : baseGridSize(gridSizeParam),
          effectiveGridSize(gridSizeParam),
          baseSpacing(spacingParam),
          baseRadius(radiusParam),
          requiredHits(requiredHitsParam),
          desiredActiveCount(desiredActiveCountParam),
          lifeTime(lifeTimeParam),
          baseColor(colorParam)
    {
    }

    void setSpacingScale(float scale) override
    {
        spacingScale = std::max(0.5f, scale);
        // 同步调整网格大小：分辨率越高，网格越大
        effectiveGridSize = std::max(baseGridSize, static_cast<int>(baseGridSize * scale));
    }

    void setGridScale(float scale) override
    {
        gridScale = std::max(0.5f, scale);
        effectiveGridSize = std::max(baseGridSize, static_cast<int>(baseGridSize * scale));
    }

    SpawnInfo createSpawn(const sf::Vector2u& areaSize,
                          const std::vector<sf::Vector2f>& occupiedPositions) override
    {
        const sf::Vector2f center(static_cast<float>(areaSize.x) * 0.5f,
                                  static_cast<float>(areaSize.y) * 0.5f);

        const float spacing = baseSpacing * spacingScale;
        const float scaledRadius = baseRadius * std::sqrt(spacingScale); // 半径适度增大

        std::vector<sf::Vector2f> candidates;
        const int gs = effectiveGridSize;
        candidates.reserve(static_cast<std::size_t>(gs * gs));

        const int halfGrid = gs / 2;
        for (int iy = -halfGrid; iy <= halfGrid; ++iy)
        {
            for (int ix = -halfGrid; ix <= halfGrid; ++ix)
            {
                const sf::Vector2f position(center.x + static_cast<float>(ix) * spacing,
                                            center.y + static_cast<float>(iy) * spacing);
                if (!isBlocked(position, occupiedPositions))
                {
                    candidates.push_back(position);
                }
            }
        }

        if (candidates.empty())
        {
            for (int iy = -halfGrid; iy <= halfGrid; ++iy)
            {
                for (int ix = -halfGrid; ix <= halfGrid; ++ix)
                {
                    candidates.emplace_back(center.x + static_cast<float>(ix) * spacing,
                                            center.y + static_cast<float>(iy) * spacing);
                }
            }
        }

        const int index = getRandomIndex(static_cast<int>(candidates.size()));
        return { candidates[index], scaledRadius, lifeTime, requiredHits, baseColor };
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
        return desiredActiveCount;
    }

private:
    bool isBlocked(const sf::Vector2f& position,
                   const std::vector<sf::Vector2f>& occupiedPositions) const
    {
        for (const auto& occupied : occupiedPositions)
        {
            if (MathUtils::distance(position, occupied) < 0.1f)
            {
                return true;
            }
        }

        return lastHitPosition && MathUtils::distance(position, *lastHitPosition) < 0.1f;
    }

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

    int baseGridSize = 3;
    int effectiveGridSize = 3;
    float baseSpacing = 120.0f;
    float baseRadius = 35.0f;
    float spacingScale = 1.0f;
    float gridScale = 1.0f;
    int requiredHits = 1;
    int desiredActiveCount = 3;
    float lifeTime = -1.0f;
    sf::Color baseColor = sf::Color::Red;
    std::optional<sf::Vector2f> lastHitPosition;
};
