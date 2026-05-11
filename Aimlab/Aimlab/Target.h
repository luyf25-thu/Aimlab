#pragma once

#include <SFML/Graphics.hpp>
#include "MathUtils.h"

// 标靶抽象基类：定义靶子的通用属性
class Target
{
public:
    virtual ~Target() = default;

    // 初始化或重置靶子状态
    virtual void init(const sf::Vector2f& startPos, float r, float lifeTime) = 0;

    // 更新靶子逻辑
    virtual void update(float deltaTime) = 0;

    // 被命中时的处理
    virtual void onHit() = 0;

    // 绘制靶子
    void render(sf::RenderWindow& window)
    {
        if (!isActive)
        {
            return;
        }
        window.draw(shape);
    }

    // 判断是否被击中
    bool isHit(const sf::Vector2f& mousePos) const
    {
        if (!isActive || isHitState)
        {
            return false;
        }
        return MathUtils::checkCirclePointCollision(position, radius, mousePos);
    }

    // 判断是否处于激活状态
    bool getIsActive() const
    {
        return isActive;
    }

    // 将靶子设为休眠
    void deactivate()
    {
        isActive = false;
    }

protected:
    sf::CircleShape shape;
    sf::Vector2f position;
    float radius = 0.0f;
    float lifeTimer = 0.0f;
    bool isActive = false;
    bool isHitState = false;
    float hitTimer = 0.0f;
    float hitDisplayDuration = 0.12f;
};
