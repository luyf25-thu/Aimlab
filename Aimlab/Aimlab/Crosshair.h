#pragma once

#include <SFML/Graphics.hpp>
#include <array>

// 瞄准镜样式准星：圆环 + 十字线（中心留空）+ 中心小点
class Crosshair
{
public:
    Crosshair()
    {
        const sf::Color mainColor(0, 255, 80, 220);     // 绿色
        const sf::Color outline(0, 0, 0, 160);           // 黑色轮廓

        // 外圆环
        outerRing.setRadius(28.0f);
        outerRing.setFillColor(sf::Color::Transparent);
        outerRing.setOutlineColor(mainColor);
        outerRing.setOutlineThickness(2.0f);

        // 内圆环
        innerRing.setRadius(8.0f);
        innerRing.setFillColor(sf::Color::Transparent);
        innerRing.setOutlineColor(mainColor);
        innerRing.setOutlineThickness(1.5f);

        // 四条十字线（中心留空，分上下左右四段）
        for (int i = 0; i < 4; ++i)
        {
            lines[i].setFillColor(mainColor);
            lines[i].setOutlineColor(outline);
            lines[i].setOutlineThickness(0.5f);
        }

        // 上线：中心上方 4px 到 28px
        lines[0].setSize({ 2.0f, 20.0f });   // 宽2, 高20
        // 下线
        lines[1].setSize({ 2.0f, 20.0f });
        // 左线
        lines[2].setSize({ 20.0f, 2.0f });
        // 右线
        lines[3].setSize({ 20.0f, 2.0f });

        // 中心小点
        centerDot.setRadius(2.0f);
        centerDot.setFillColor(sf::Color(255, 255, 255, 240));
        centerDot.setOutlineColor(outline);
        centerDot.setOutlineThickness(0.5f);
    }

    void updatePosition(const sf::Vector2f& basePos)
    {
        const float cx = basePos.x;
        const float cy = basePos.y;

        outerRing.setPosition({ cx - 28.0f, cy - 28.0f });
        innerRing.setPosition({ cx - 8.0f, cy - 8.0f });

        // 上线：从圆心上方4px开始往上
        lines[0].setPosition({ cx - 1.0f, cy - 24.0f });
        // 下线：从圆心下方4px开始往下
        lines[1].setPosition({ cx - 1.0f, cy + 4.0f });
        // 左线：从圆心左侧4px开始往左
        lines[2].setPosition({ cx - 24.0f, cy - 1.0f });
        // 右线：从圆心右侧4px开始往右
        lines[3].setPosition({ cx + 4.0f, cy - 1.0f });

        centerDot.setPosition({ cx - 2.0f, cy - 2.0f });
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(outerRing);
        window.draw(innerRing);
        for (auto& line : lines) window.draw(line);
        window.draw(centerDot);
    }

private:
    sf::CircleShape outerRing;
    sf::CircleShape innerRing;
    std::array<sf::RectangleShape, 4> lines;
    sf::CircleShape centerDot;
};
