#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <random>

// 数学工具类：提供距离计算、碰撞检测、随机数
class MathUtils
{
public:
    // 计算两点之间的距离
    static float distance(const sf::Vector2f& p1, const sf::Vector2f& p2)
    {
        const float dx = p1.x - p2.x;
        const float dy = p1.y - p2.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    // 判断圆与点是否碰撞
    static bool checkCirclePointCollision(const sf::Vector2f& center, float radius, const sf::Vector2f& point)
    {
        return distance(center, point) <= radius;
    }

    // 生成指定范围内的随机浮点数
    static float getRandomFloat(float minValue, float maxValue)
    {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(getRng());
    }

private:
    // 获取随机数引擎
    static std::mt19937& getRng()
    {
        static std::mt19937 rng(std::random_device{}());
        return rng;
    }
};
