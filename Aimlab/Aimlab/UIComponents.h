#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <optional>
#include "UITheme.h"

// ============================================================
// UIPanel — 半透明矩形面板，带边框
// ============================================================
class UIPanel
{
public:
    UIPanel() = default;

    UIPanel(const sf::Vector2f& size, const sf::Vector2f& position,
            const sf::Color& fillColor = UITheme::PanelBg,
            const sf::Color& borderColor = UITheme::PanelBorder,
            float borderWidth = UITheme::PanelBorderWidth)
    {
        setSize(size);
        setPosition(position);
        setFillColor(fillColor);
        setBorder(borderColor, borderWidth);
    }

    void setSize(const sf::Vector2f& size)      { bg.setSize(size); }
    void setPosition(const sf::Vector2f& pos)    { bg.setPosition(pos); }
    void setFillColor(const sf::Color& color)    { bg.setFillColor(color); }
    void setBorder(const sf::Color& color, float thickness)
    {
        bg.setOutlineColor(color);
        bg.setOutlineThickness(thickness);
    }
    sf::Vector2f getPosition() const             { return bg.getPosition(); }
    sf::Vector2f getSize() const                 { return bg.getSize(); }
    sf::FloatRect getBounds() const              { return bg.getGlobalBounds(); }
    void render(sf::RenderWindow& window)        { window.draw(bg); }

private:
    sf::RectangleShape bg;
};


// ============================================================
// UIButton — 可交互按钮（悬停高亮 + 点击回调）
// ============================================================
class UIButton
{
public:
    using Callback = std::function<void()>;

    UIButton() = default;

    // 构造按钮：文字、字号、尺寸、位置
    UIButton(const std::string& label, unsigned int charSize,
             const sf::Vector2f& size, const sf::Vector2f& position)
    {
        labelText.emplace(*UITheme::DefaultFont);
        setLabel(label, charSize);
        setSize(size);
        setPosition(position);
        applyNormalStyle();
    }

    void setLabel(const std::string& text, unsigned int charSize = 22)
    {
        if (!labelText) return;
        labelText->setFont(*UITheme::DefaultFont);
        labelText->setString(text);
        labelText->setCharacterSize(charSize);
        centerText();
    }

    void setFontSize(unsigned int size)
    {
        if (!labelText) return;
        labelText->setCharacterSize(size);
        centerText();
    }

    void setSize(const sf::Vector2f& size)       { bg.setSize(size); }
    void setPosition(const sf::Vector2f& pos)    { bg.setPosition(pos); centerText(); }
    sf::Vector2f getPosition() const             { return bg.getPosition(); }
    sf::Vector2f getSize() const                 { return bg.getSize(); }
    sf::FloatRect getBounds() const              { return bg.getGlobalBounds(); }
    void setCallback(Callback cb)                { callback = std::move(cb); }

    bool handleMouseMove(const sf::Vector2i& mousePos)
    {
        const bool inside = getBounds().contains({ static_cast<float>(mousePos.x),
                                                   static_cast<float>(mousePos.y) });
        inside ? applyHoverStyle() : applyNormalStyle();
        return inside;
    }

    bool handleClick(const sf::Vector2i& mousePos)
    {
        if (getBounds().contains({ static_cast<float>(mousePos.x),
                                   static_cast<float>(mousePos.y) }))
        {
            applyPressedStyle();
            if (callback) callback();
            return true;
        }
        return false;
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(bg);
        if (labelText) window.draw(*labelText);
    }

private:
    void centerText()
    {
        if (!labelText) return;
        const sf::FloatRect textBounds = labelText->getLocalBounds();
        const sf::Vector2f btnSize = bg.getSize();
        const sf::Vector2f btnPos = bg.getPosition();
        labelText->setOrigin({ textBounds.position.x + textBounds.size.x * 0.5f,
                               textBounds.position.y + textBounds.size.y * 0.5f });
        labelText->setPosition({ btnPos.x + btnSize.x * 0.5f,
                                 btnPos.y + btnSize.y * 0.5f });
    }

    void applyNormalStyle()
    {
        bg.setFillColor(UITheme::BtnNormal);
        bg.setOutlineColor(UITheme::BtnBorder);
        bg.setOutlineThickness(1.2f);
        if (labelText) labelText->setFillColor(UITheme::TextWhite);
    }
    void applyHoverStyle()
    {
        bg.setFillColor(UITheme::BtnHover);
        bg.setOutlineColor(UITheme::BtnBorderHover);
        bg.setOutlineThickness(2.0f);
        if (labelText) labelText->setFillColor(sf::Color::White);
    }
    void applyPressedStyle()
    {
        bg.setFillColor(UITheme::BtnPressed);
        bg.setOutlineColor(UITheme::BtnBorderHover);
        bg.setOutlineThickness(1.5f);
    }

    sf::RectangleShape bg;
    std::optional<sf::Text> labelText;
    Callback callback;
};


// ============================================================
// UIText — 文本标签（支持对齐）
// ============================================================
class UIText
{
public:
    enum class Align { Left, Center, Right };

    UIText() = default;

    UIText(const std::string& str, unsigned int charSize,
           const sf::Vector2f& position,
           const sf::Color& color = UITheme::TextWhite,
           Align align = Align::Left)
    {
        text.emplace(*UITheme::DefaultFont);
        setString(str, charSize);
        setColor(color);
        setPosition(position, align);
    }

    void setString(const std::string& str, unsigned int charSize = 20)
    {
        if (!text) return;
        text->setFont(*UITheme::DefaultFont);
        text->setString(str);
        text->setCharacterSize(charSize);
    }

    void setColor(const sf::Color& color) { if (text) text->setFillColor(color); }
    void setPosition(const sf::Vector2f& pos, Align align = Align::Left)
    {
        if (!text) return;
        const sf::FloatRect bounds = text->getLocalBounds();
        switch (align)
        {
        case Align::Left:
            text->setOrigin({ bounds.position.x, bounds.position.y }); break;
        case Align::Center:
            text->setOrigin({ bounds.position.x + bounds.size.x * 0.5f,
                              bounds.position.y + bounds.size.y * 0.5f }); break;
        case Align::Right:
            text->setOrigin({ bounds.position.x + bounds.size.x,
                              bounds.position.y }); break;
        }
        text->setPosition(pos);
    }

    sf::FloatRect getBounds() const { return text ? text->getGlobalBounds() : sf::FloatRect(); }
    void render(sf::RenderWindow& window) { if (text) window.draw(*text); }

private:
    std::optional<sf::Text> text;
};


// ============================================================
// 辅助绘制函数
// ============================================================

// 绘制水平进度条
inline void drawProgressBar(sf::RenderWindow& window,
                            const sf::Vector2f& position,
                            float width, float height,
                            float ratio,
                            const sf::Color& bgColor,
                            const sf::Color& fillColor,
                            const sf::Color& borderColor = UITheme::PanelBorder)
{
    sf::RectangleShape barBg({ width, height });
    barBg.setPosition(position);
    barBg.setFillColor(bgColor);
    barBg.setOutlineColor(borderColor);
    barBg.setOutlineThickness(1.0f);
    window.draw(barBg);

    if (ratio > 0.0f)
    {
        const float fillW = (width - 4.0f) * std::clamp(ratio, 0.0f, 1.0f);
        sf::RectangleShape barFill({ fillW, height - 4.0f });
        barFill.setPosition({ position.x + 2.0f, position.y + 2.0f });
        barFill.setFillColor(ratio < 0.25f ? UITheme::AmmoLow : fillColor);
        window.draw(barFill);
    }
}

// 绘制装饰线
inline void drawSeparatorLine(sf::RenderWindow& window,
                              const sf::Vector2f& start, float width,
                              const sf::Color& color = UITheme::AccentBlue)
{
    sf::RectangleShape line({ width, 2.0f });
    line.setPosition(start);
    line.setFillColor(color);
    window.draw(line);
}
