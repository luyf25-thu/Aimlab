#pragma once

#include <SFML/Graphics.hpp>
#include "Weapon.h"

// 准星类：绘制鼠标位置并叠加后坐力
class Crosshair
{
public:
    // 初始化一个简单的准星贴图
    Crosshair()
        : texture(), sprite(texture)
    {
        sf::Image image({ 8, 8 }, sf::Color::Transparent);
        for (unsigned int i = 0; i < 8; ++i)
        {
            image.setPixel({ i, 4 }, sf::Color::Black);
            image.setPixel({ 4, i }, sf::Color::Black);
        }
        texture.loadFromImage(image);
        sprite.setTexture(texture, true);
        sprite.setOrigin({ 4.0f, 4.0f });
    }

    // 绑定当前武器
    void setWeapon(Weapon* weapon)
    {
        currentWeapon = weapon;
    }

    // 更新准星显示位置
    void updatePosition(const sf::Vector2f& basePos)
    {
        sprite.setPosition(basePos);
    }

    // 获取准星的实际世界坐标
    sf::Vector2f getWorldPosition() const
    {
        return sprite.getPosition();
    }

    // 渲染准星
    void render(sf::RenderWindow& window)
    {
        window.draw(sprite);
    }

private:
    sf::Texture texture;
    sf::Sprite sprite;
    Weapon* currentWeapon = nullptr;
};
