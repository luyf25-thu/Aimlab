#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>

// 生成信息：位置、半径、存活时间
struct SpawnInfo
{
    sf::Vector2f position;
    float radius = 0.0f;
    float lifeTime = 0.0f;
    int requiredHits = 1;
    sf::Color baseColor = sf::Color::Red;
};

// 游戏模式基类：定义靶子生成规则
class GameMode
{
public:
    virtual ~GameMode() = default;

    // 创建一次靶子生成参数
    virtual SpawnInfo createSpawn(const sf::Vector2u& areaSize,
                                  const std::vector<sf::Vector2f>& occupiedPositions) = 0;

    // 命中回调（用于记录最近命中位置）
    virtual void onTargetHit(const sf::Vector2f& position)
    {
        (void)position;
    }

    // 重置模式状态
    virtual void reset()
    {
    }

    // 获取期望同时存在的靶子数量
    virtual int getDesiredActiveCount() const
    {
        return 1;
    }
};
