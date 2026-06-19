#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <array>
#include <functional>
#include <optional>
#include "UITheme.h"
#include "UIComponents.h"
#include "GameState.h"

class HelpScreen
{
public:
    using StateCallback = std::function<void(GameState)>;

    HelpScreen() = default;

    void init()
    {
        overlayBg.setFillColor(UITheme::OverlayBg);
        panelBg.setFillColor(UITheme::PanelBg);
        panelBg.setOutlineColor(UITheme::PanelBorder);
        panelBg.setOutlineThickness(UITheme::PanelBorderWidth);

        titleText.emplace(*UITheme::DefaultFont);
        titleText->setString("Controls & Info");
        titleText->setCharacterSize(34);
        titleText->setFillColor(UITheme::TextWhite);
        titleText->setStyle(sf::Text::Bold);

        for (int i = 0; i < 3; ++i)
        {
            sectionHeadings[i].emplace(*UITheme::DefaultFont);
            sectionHeadings[i]->setCharacterSize(18);
            sectionHeadings[i]->setFillColor(UITheme::AccentBlue);
            sectionHeadings[i]->setStyle(sf::Text::Bold);
        }
        sectionHeadings[0]->setString("Controls");
        sectionHeadings[1]->setString("Game Modes");
        sectionHeadings[2]->setString("Weapons");
        separator1.setFillColor(UITheme::PanelBorder);
        separator2.setFillColor(UITheme::PanelBorder);

        const struct { const char* k; const char* d; } bindings[] = {
            { "Mouse Move", "Aim / Look Around" }, { "Left Click", "Fire" },
            { "1 / 2 / 3", "Switch Weapon (USP / AK-47 / M4A1)" }, { "M", "Switch Game Mode" },
            { "X", "Toggle Infinite Ammo" }, { "R", "Reload" }, { "ESC", "Pause / Back" },
        };
        for (int i = 0; i < 7; ++i)
        {
            keyRows[i].key.emplace(*UITheme::DefaultFont);
            keyRows[i].key->setString(bindings[i].k); keyRows[i].key->setCharacterSize(16); keyRows[i].key->setFillColor(UITheme::AccentOrange);
            keyRows[i].dash.emplace(*UITheme::DefaultFont);
            keyRows[i].dash->setString("-"); keyRows[i].dash->setCharacterSize(16); keyRows[i].dash->setFillColor(UITheme::TextDim);
            keyRows[i].desc.emplace(*UITheme::DefaultFont);
            keyRows[i].desc->setString(bindings[i].d); keyRows[i].desc->setCharacterSize(16); keyRows[i].desc->setFillColor(UITheme::TextGray);
        }

        const char* modes[] = { "Simple — 3x3 grid, large targets, warm-up", "Hard — 5x5 grid, small targets, precision", "Recoil — Multi-hit targets, recoil control" };
        for (int i = 0; i < 3; ++i) {
            modeRows[i].desc.emplace(*UITheme::DefaultFont);
            modeRows[i].desc->setString(modes[i]); modeRows[i].desc->setCharacterSize(15); modeRows[i].desc->setFillColor(UITheme::TextGray);
        }

        const char* weapons[] = { "USP — Pistol, Semi-auto, 12 rounds, low recoil", "AK-47 — Assault Rifle, Full-auto, 30 rounds, high recoil", "M4A1 — Carbine, Full-auto, 30 rounds, medium recoil" };
        for (int i = 0; i < 3; ++i) {
            weaponRows[i].desc.emplace(*UITheme::DefaultFont);
            weaponRows[i].desc->setString(weapons[i]); weaponRows[i].desc->setCharacterSize(15); weaponRows[i].desc->setFillColor(UITheme::TextGray);
        }

        hintText.emplace(*UITheme::DefaultFont);
        hintText->setString("Press ESC or click Back to return"); hintText->setCharacterSize(13); hintText->setFillColor(UITheme::TextDim);

        btnBack = UIButton("Back", 20, { 160.0f, 40.0f }, { 0, 0 });
        btnBack.setCallback([this]() { if (onStateChange) onStateChange(GameState::MainMenu); });
    }

    void setStateCallback(StateCallback cb) { onStateChange = std::move(cb); }
    void onResize(const sf::Vector2u& windowSize) { viewSize = windowSize; layout(); }

    void handleEvent(const sf::Event& event)
    {
        if (const auto* moved = event.getIf<sf::Event::MouseMoved>()) {
            const sf::Vector2i mp = { moved->position.x, moved->position.y }; btnBack.handleMouseMove(mp);
        } else if (const auto* pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (pressed->button == sf::Mouse::Button::Left) {
                const sf::Vector2i mp = { pressed->position.x, pressed->position.y }; btnBack.handleClick(mp);
            }
        } else if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::Backspace)
                if (onStateChange) onStateChange(GameState::MainMenu);
        }
    }

    void render(sf::RenderWindow& window)
    {
        // 视图已由 Game 设置
        window.draw(overlayBg); window.draw(panelBg);
        if (titleText) window.draw(*titleText);
        for (const auto& h : sectionHeadings) if (h) window.draw(*h);
        for (const auto& row : keyRows) { if (row.key) window.draw(*row.key); if (row.dash) window.draw(*row.dash); if (row.desc) window.draw(*row.desc); }
        window.draw(separator1); window.draw(separator2);
        for (const auto& row : modeRows) if (row.desc) window.draw(*row.desc);
        for (const auto& row : weaponRows) if (row.desc) window.draw(*row.desc);
        if (hintText) window.draw(*hintText);
        btnBack.render(window);
    }

private:
    void layout()
    {
        const float w = static_cast<float>(viewSize.x), h = static_cast<float>(viewSize.y), cx = w * 0.5f;
        overlayBg.setSize({ w, h }); overlayBg.setPosition({ 0.0f, 0.0f });
        const float panelW = 580.0f, panelH = 530.0f, panelTop = (h - panelH) * 0.5f;
        panelBg.setSize({ panelW, panelH }); panelBg.setPosition({ cx - panelW * 0.5f, panelTop });
        if (titleText) { const auto& tb = titleText->getLocalBounds(); titleText->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y }); titleText->setPosition({ cx, panelTop + 20.0f }); }
        const float leftX = cx - panelW * 0.5f + 40.0f; float curY = panelTop + 70.0f;
        if (sectionHeadings[0]) { const auto& sb = sectionHeadings[0]->getLocalBounds(); sectionHeadings[0]->setOrigin({ sb.position.x, sb.position.y }); sectionHeadings[0]->setPosition({ leftX, curY }); }
        curY += 28.0f;
        for (int i = 0; i < 7; ++i) {
            if (keyRows[i].key) { const auto& kb = keyRows[i].key->getLocalBounds(); keyRows[i].key->setOrigin({ kb.position.x, kb.position.y }); keyRows[i].key->setPosition({ leftX + 10.0f, curY }); }
            if (keyRows[i].dash) { const auto& db = keyRows[i].dash->getLocalBounds(); keyRows[i].dash->setOrigin({ db.position.x, db.position.y }); keyRows[i].dash->setPosition({ leftX + 140.0f, curY }); }
            if (keyRows[i].desc) { const auto& sb = keyRows[i].desc->getLocalBounds(); keyRows[i].desc->setOrigin({ sb.position.x, sb.position.y }); keyRows[i].desc->setPosition({ leftX + 160.0f, curY }); }
            curY += 24.0f;
        }
        curY += 8.0f; separator1.setSize({ panelW - 80.0f, 1.5f }); separator1.setPosition({ leftX, curY }); curY += 18.0f;
        if (sectionHeadings[1]) { const auto& sb = sectionHeadings[1]->getLocalBounds(); sectionHeadings[1]->setOrigin({ sb.position.x, sb.position.y }); sectionHeadings[1]->setPosition({ leftX, curY }); }
        curY += 28.0f;
        for (int i = 0; i < 3; ++i) { if (modeRows[i].desc) { const auto& mb = modeRows[i].desc->getLocalBounds(); modeRows[i].desc->setOrigin({ mb.position.x, mb.position.y }); modeRows[i].desc->setPosition({ leftX + 10.0f, curY }); } curY += 22.0f; }
        curY += 8.0f; separator2.setSize({ panelW - 80.0f, 1.5f }); separator2.setPosition({ leftX, curY }); curY += 18.0f;
        if (sectionHeadings[2]) { const auto& sb = sectionHeadings[2]->getLocalBounds(); sectionHeadings[2]->setOrigin({ sb.position.x, sb.position.y }); sectionHeadings[2]->setPosition({ leftX, curY }); }
        curY += 28.0f;
        for (int i = 0; i < 3; ++i) { if (weaponRows[i].desc) { const auto& wb = weaponRows[i].desc->getLocalBounds(); weaponRows[i].desc->setOrigin({ wb.position.x, wb.position.y }); weaponRows[i].desc->setPosition({ leftX + 10.0f, curY }); } curY += 22.0f; }
        if (hintText) { const auto& hb = hintText->getLocalBounds(); hintText->setOrigin({ hb.position.x + hb.size.x * 0.5f, hb.position.y + hb.size.y }); hintText->setPosition({ cx, panelTop + panelH - 52.0f }); }
        btnBack.setPosition({ cx - 80.0f, panelTop + panelH - 50.0f });
    }

    sf::RectangleShape overlayBg, panelBg;
    std::optional<sf::Text> titleText;
    std::array<std::optional<sf::Text>, 3> sectionHeadings;
    struct KeyRow { std::optional<sf::Text> key, dash, desc; };
    std::array<KeyRow, 7> keyRows;
    sf::RectangleShape separator1, separator2;
    struct InfoRow { std::optional<sf::Text> desc; };
    std::array<InfoRow, 3> modeRows, weaponRows;
    std::optional<sf::Text> hintText;
    UIButton btnBack;
    sf::Vector2u viewSize{ 800, 600 };
    StateCallback onStateChange;
};
