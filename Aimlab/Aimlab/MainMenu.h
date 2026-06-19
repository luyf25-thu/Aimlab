#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
#include <filesystem>
#include <optional>
#include "UITheme.h"
#include "UIComponents.h"
#include "GameState.h"

// ============================================================
// MainMenu — 主菜单界面
// ============================================================
class MainMenu
{
public:
    using StateCallback = std::function<void(GameState)>;

    MainMenu() = default;

    bool init(const std::filesystem::path& bgPath)
    {
        if (!bgTexture.loadFromFile(bgPath.u8string()))
        {
            return false;
        }
        bgSprite.emplace(bgTexture);
        createElements();
        return true;
    }

    void setStateCallback(StateCallback cb) { onStateChange = std::move(cb); }

    void onResize(const sf::Vector2u& windowSize)
    {
        viewSize = windowSize;
        updateBackgroundScale();
        layoutElements();
    }

    void handleEvent(const sf::Event& event)
    {
        if (const auto* moved = event.getIf<sf::Event::MouseMoved>())
        {
            const sf::Vector2i mp = { moved->position.x, moved->position.y };
            btnStart.handleMouseMove(mp);
            btnHelp.handleMouseMove(mp);
            btnExit.handleMouseMove(mp);
        }
        else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>())
        {
            if (pressed->button == sf::Mouse::Button::Left)
            {
                const sf::Vector2i mp = { pressed->position.x, pressed->position.y };
                btnStart.handleClick(mp);
                btnHelp.handleClick(mp);
                btnExit.handleClick(mp);
            }
        }
    }

    void render(sf::RenderWindow& window)
    {
        if (bgSprite) window.draw(*bgSprite);
        window.draw(overlayBg);
        if (titleText) window.draw(*titleText);
        if (subtitleText) window.draw(*subtitleText);
        window.draw(titleLine);
        btnStart.render(window);
        btnHelp.render(window);
        btnExit.render(window);
        if (versionText) window.draw(*versionText);
    }

private:
    void createElements()
    {
        titleText.emplace(*UITheme::DefaultFont);
        titleText->setString("AIM TRAINER");
        titleText->setCharacterSize(52);
        titleText->setFillColor(UITheme::TextWhite);
        titleText->setStyle(sf::Text::Bold);

        subtitleText.emplace(*UITheme::DefaultFont);
        subtitleText->setString("FPS Aim Trainer");
        subtitleText->setCharacterSize(24);
        subtitleText->setFillColor(UITheme::AccentBlue);

        titleLine.setFillColor(UITheme::AccentBlue);

        versionText.emplace(*UITheme::DefaultFont);
        versionText->setString("v1.0  |  OOP Course Project");
        versionText->setCharacterSize(13);
        versionText->setFillColor(UITheme::TextDim);

        const sf::Vector2f btnSize{ UITheme::ButtonMinWidth, UITheme::ButtonHeight };

        btnStart = UIButton("Start", 24, btnSize, { 0, 0 });
        btnStart.setCallback([this]() { if (onStateChange) onStateChange(GameState::Playing); });

        btnHelp = UIButton("Help", 24, btnSize, { 0, 0 });
        btnHelp.setCallback([this]() { if (onStateChange) onStateChange(GameState::Help); });

        btnExit = UIButton("Exit", 24, btnSize, { 0, 0 });
        btnExit.setCallback([this]() { if (onStateChange) onStateChange(GameState::Exit); });
    }

    void updateBackgroundScale()
    {
        const sf::Vector2u texSize = bgTexture.getSize();
        if (texSize.x == 0 || texSize.y == 0) return;

        const float scale = std::max(
            static_cast<float>(viewSize.x) / static_cast<float>(texSize.x),
            static_cast<float>(viewSize.y) / static_cast<float>(texSize.y));

        if (bgSprite) bgSprite->setScale({ scale, scale });
        const float sw = static_cast<float>(texSize.x) * scale;
        const float sh = static_cast<float>(texSize.y) * scale;
        if (bgSprite) bgSprite->setPosition({
            (static_cast<float>(viewSize.x) - sw) * 0.5f,
            (static_cast<float>(viewSize.y) - sh) * 0.5f });

        overlayBg.setSize({ static_cast<float>(viewSize.x), static_cast<float>(viewSize.y) });
        overlayBg.setPosition({ 0.0f, 0.0f });
        overlayBg.setFillColor(sf::Color(0, 0, 0, 120));
    }

    void layoutElements()
    {
        const float w = static_cast<float>(viewSize.x);
        const float h = static_cast<float>(viewSize.y);
        const float cx = w * 0.5f;
        const float cy = h * 0.5f;

        // 按窗口比例缩放
        const float btnW = w * 0.28f;
        const float btnH = h * 0.065f;
        const float titleSize = h * 0.075f;
        const float titleY = cy - h * 0.22f;
        const float subY = cy - h * 0.12f;
        const float lineY = cy - h * 0.08f;
        const float btn1Y = cy - h * 0.02f;
        const float btn2Y = cy + h * 0.06f;
        const float btn3Y = cy + h * 0.14f;

        if (titleText) titleText->setCharacterSize(static_cast<unsigned int>(titleSize));
        if (subtitleText) subtitleText->setCharacterSize(static_cast<unsigned int>(titleSize * 0.4f));

        btnStart.setSize({ btnW, btnH });
        btnHelp.setSize({ btnW, btnH });
        btnExit.setSize({ btnW, btnH });

        // 按钮字体按窗口比例缩放
        const unsigned int btnFontSize = static_cast<unsigned int>(h * 0.032f);
        btnStart.setFontSize(btnFontSize);
        btnHelp.setFontSize(btnFontSize);
        btnExit.setFontSize(btnFontSize);

        if (titleText)
        {
            const sf::FloatRect tb = titleText->getLocalBounds();
            titleText->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y + tb.size.y * 0.5f });
            titleText->setPosition({ cx, titleY });
        }
        if (subtitleText)
        {
            const sf::FloatRect sb = subtitleText->getLocalBounds();
            subtitleText->setOrigin({ sb.position.x + sb.size.x * 0.5f, sb.position.y + sb.size.y * 0.5f });
            subtitleText->setPosition({ cx, subY });
        }

        titleLine.setSize({ w * 0.12f, 3.0f });
        titleLine.setPosition({ cx - w * 0.06f, lineY });

        const float btnX = cx - btnW * 0.5f;
        btnStart.setPosition({ btnX, btn1Y });
        btnHelp.setPosition({ btnX, btn2Y });
        btnExit.setPosition({ btnX, btn3Y });

        if (versionText)
        {
            const sf::FloatRect vb = versionText->getLocalBounds();
            versionText->setOrigin({ vb.position.x + vb.size.x * 0.5f, vb.position.y + vb.size.y });
            versionText->setPosition({ cx, h - 16.0f });
        }
    }

    sf::Texture bgTexture;
    std::optional<sf::Sprite> bgSprite;
    sf::RectangleShape overlayBg;

    std::optional<sf::Text> titleText;
    std::optional<sf::Text> subtitleText;
    sf::RectangleShape titleLine;

    UIButton btnStart;
    UIButton btnHelp;
    UIButton btnExit;

    std::optional<sf::Text> versionText;

    sf::Vector2u viewSize{ 800, 600 };
    StateCallback onStateChange;
};
