#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <array>
#include <sstream>
#include <iomanip>
#include <optional>
#include "UITheme.h"
#include "UIComponents.h"

// ============================================================
// GameHUD — 游戏内抬头显示
// ============================================================
class GameHUD
{
public:
    struct WeaponInfo
    {
        std::string shortName;
        std::string fullName;
        std::string fireType;
        int keyIndex = 0;
        bool isActive = false;
        int currentAmmo = 0;
        int ammoCapacity = 0;
        bool isReloading = false;
        bool isInfiniteAmmo = false;
    };

    struct StatsInfo
    {
        int hits = 0;
        int misses = 0;
        int totalShots = 0;
        float accuracy = 0.0f;
        float avgSecondsPerHit = 0.0f;
    };

    struct ModeInfo
    {
        std::string name;
        int index = 0;
        int totalModes = 3;
    };

    GameHUD()
    {
        initPanels();
        initWeaponSlots();
        needsTextInit = true;
    }

    // 在字体加载后调用
    void initTexts()
    {
        if (!needsTextInit) return;
        needsTextInit = false;

        auto createText = [&](std::optional<sf::Text>& t, unsigned int size, const sf::Color& color) {
            t.emplace(*UITheme::DefaultFont);
            t->setCharacterSize(size);
            t->setFillColor(color);
        };

        createText(modeNameText, 18, UITheme::TextWhite);
        createText(timerText, 20, UITheme::AccentBlue);
        for (auto& t : topRightTexts) createText(t, 16, UITheme::TextGray);
        for (auto& t : weaponSlotLabels) createText(t, 13, UITheme::TextGray);
        for (auto& t : weaponKeyHints) createText(t, 11, UITheme::TextDim);
        for (auto& t : bottomTexts) createText(t, 16, UITheme::TextWhite);
    }

    void update(const WeaponInfo& active, const std::array<WeaponInfo, 3>& allWeapons,
                const StatsInfo& stats, const ModeInfo& mode, float timeLeft,
                const sf::Vector2u& winSize)
    {
        updateTopBar(mode, stats, timeLeft, winSize);
        updateBottomBar(active, allWeapons, winSize);
    }

    void render(sf::RenderWindow& window)
    {
        window.draw(topBarBg);
        window.draw(modeIndicator);
        if (modeNameText) window.draw(*modeNameText);
        if (timerText) window.draw(*timerText);
        for (auto& t : topRightTexts) if (t) window.draw(*t);

        window.draw(bottomBarBg);
        window.draw(bottomAccentLine);
        for (auto& slot : weaponSlots) window.draw(slot.bg);

        window.draw(ammoBarBg);
        window.draw(ammoBarFill);

        for (auto& t : bottomTexts) if (t) window.draw(*t);
        for (auto& t : weaponSlotLabels) if (t) window.draw(*t);
        for (auto& t : weaponKeyHints) if (t) window.draw(*t);
    }

private:
    void initPanels()
    {
        topBarBg.setFillColor(UITheme::PanelBg);
        topBarBg.setOutlineColor(UITheme::PanelBorder);
        topBarBg.setOutlineThickness(UITheme::PanelBorderWidth);

        bottomBarBg.setFillColor(UITheme::PanelBg);
        bottomBarBg.setOutlineColor(UITheme::PanelBorder);
        bottomBarBg.setOutlineThickness(UITheme::PanelBorderWidth);

        bottomAccentLine.setFillColor(UITheme::AccentBlue);
    }

    void initWeaponSlots()
    {
        for (int i = 0; i < 3; ++i)
        {
            weaponSlots[i].bg.setOutlineThickness(1.5f);
        }
    }

    void updateTopBar(const ModeInfo& mode, const StatsInfo& stats, float timeLeft,
                      const sf::Vector2u& winSize)
    {
        const float w = static_cast<float>(winSize.x);
        const float h = static_cast<float>(winSize.y);
        const float barH = h * 0.058f;
        topBarBg.setSize({ w, barH });
        topBarBg.setPosition({ 0.0f, 0.0f });

        const float modeLeftX = w * 0.02f;
        const float modeCenterY = barH * 0.5f;

        const float indicatorH = barH * 0.4f;
        modeIndicator.setSize({ 4.0f, indicatorH });
        modeIndicator.setPosition({ modeLeftX, modeCenterY - indicatorH * 0.5f });
        switch (mode.index)
        {
        case 0: modeIndicator.setFillColor(UITheme::AccentGreen); break;
        case 1: modeIndicator.setFillColor(UITheme::AccentOrange); break;
        case 2: modeIndicator.setFillColor(UITheme::AccentRed); break;
        default: modeIndicator.setFillColor(UITheme::AccentBlue); break;
        }

        if (modeNameText)
        {
            modeNameText->setString(mode.name);
            modeNameText->setCharacterSize(static_cast<unsigned int>(barH * 0.4f));
            const sf::FloatRect mb = modeNameText->getLocalBounds();
            modeNameText->setOrigin({ mb.position.x, mb.position.y + mb.size.y * 0.5f });
            modeNameText->setPosition({ modeLeftX + 12.0f, modeCenterY });
        }

        // 倒计时（正中）
        if (timerText)
        {
            std::ostringstream oss;
            const int secs = static_cast<int>(std::ceil(timeLeft));
            oss << secs << "s";
            timerText->setString(oss.str());
            timerText->setCharacterSize(static_cast<unsigned int>(barH * 0.5f));
            timerText->setFillColor(timeLeft <= 10.0f ? UITheme::AccentRed : UITheme::AccentBlue);
            timerText->setStyle(timeLeft <= 10.0f ? sf::Text::Bold : sf::Text::Regular);
            const sf::FloatRect tb = timerText->getLocalBounds();
            timerText->setOrigin({ tb.position.x + tb.size.x * 0.5f, tb.position.y + tb.size.y * 0.5f });
            timerText->setPosition({ w * 0.5f, modeCenterY });
        }

        auto setTopRight = [&](int idx, const std::string& s, const sf::Color& c = UITheme::TextGray) {
            if (!topRightTexts[idx]) return;
            topRightTexts[idx]->setString(s);
            topRightTexts[idx]->setFillColor(c);
        };

        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << stats.accuracy << "%";
            sf::Color c = stats.accuracy >= 70.0f ? UITheme::AccentGreen
                        : stats.accuracy >= 40.0f ? UITheme::AccentOrange : UITheme::AccentRed;
            setTopRight(0, oss.str(), c);
        }
        {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << stats.avgSecondsPerHit << "s";
            setTopRight(1, stats.hits > 0 ? oss.str() : "--");
        }
        setTopRight(2, std::to_string(stats.hits) + "/" + std::to_string(stats.totalShots));

        float rightX = w - w * 0.02f;
        const unsigned int statsFontSize = static_cast<unsigned int>(barH * 0.38f);
        for (int i = 2; i >= 0; --i)
        {
            if (!topRightTexts[i]) continue;
            topRightTexts[i]->setCharacterSize(statsFontSize);
            const sf::FloatRect b = topRightTexts[i]->getLocalBounds();
            topRightTexts[i]->setOrigin({ b.position.x + b.size.x, b.position.y + b.size.y * 0.5f });
            topRightTexts[i]->setPosition({ rightX, modeCenterY });
            rightX -= (b.size.x + w * 0.035f);
        }
    }

    void updateBottomBar(const WeaponInfo& active, const std::array<WeaponInfo, 3>& allWeapons,
                         const sf::Vector2u& winSize)
    {
        const float w = static_cast<float>(winSize.x);
        const float h = static_cast<float>(winSize.y);
        const float barH = h * 0.105f;               // 底部栏高度按比例
        const float barY = h - barH;

        bottomBarBg.setSize({ w, barH });
        bottomBarBg.setPosition({ 0.0f, barY });

        bottomAccentLine.setSize({ w, 2.0f });
        bottomAccentLine.setPosition({ 0.0f, barY });

        // 武器槽按比例缩放
        const float slotW = w * 0.08f, slotH = barH * 0.78f;
        const float slotStartX = w * 0.02f;
        const float slotSpacing = w * 0.008f;
        const float slotY = barY + (barH - slotH) * 0.5f;

        for (int i = 0; i < 3; ++i)
        {
            auto& slot = weaponSlots[i];
            const float sx = slotStartX + static_cast<float>(i) * (slotW + slotSpacing);
            slot.bg.setSize({ slotW, slotH });
            slot.bg.setPosition({ sx, slotY });

            if (allWeapons[i].isActive)
            {
                slot.bg.setFillColor(UITheme::WeaponActive);
                slot.bg.setOutlineColor(UITheme::AccentBlue);
                slot.bg.setOutlineThickness(2.0f);
            }
            else
            {
                slot.bg.setFillColor(UITheme::WeaponInactive);
                slot.bg.setOutlineColor(UITheme::PanelBorder);
                slot.bg.setOutlineThickness(1.0f);
            }

            if (weaponSlotLabels[i])
            {
                weaponSlotLabels[i]->setString(allWeapons[i].shortName);
                weaponSlotLabels[i]->setCharacterSize(static_cast<unsigned int>(slotH * 0.22f));
                weaponSlotLabels[i]->setFillColor(allWeapons[i].isActive ? UITheme::AccentBlue : UITheme::TextGray);
                const sf::FloatRect lb = weaponSlotLabels[i]->getLocalBounds();
                weaponSlotLabels[i]->setOrigin({ lb.position.x + lb.size.x * 0.5f, lb.position.y + lb.size.y * 0.5f });
                weaponSlotLabels[i]->setPosition({ sx + slotW * 0.5f, slotY + slotH * 0.5f - 4.0f });
            }

            if (weaponKeyHints[i])
            {
                weaponKeyHints[i]->setString("[" + std::to_string(i + 1) + "]");
                weaponKeyHints[i]->setCharacterSize(static_cast<unsigned int>(slotH * 0.18f));
                const sf::FloatRect kb = weaponKeyHints[i]->getLocalBounds();
                weaponKeyHints[i]->setOrigin({ kb.position.x + kb.size.x * 0.5f, kb.position.y + kb.size.y * 0.5f });
                weaponKeyHints[i]->setPosition({ sx + slotW * 0.5f, slotY + slotH - 12.0f });
            }
        }

        // 弹药区域（比例化）
        const float ammoBarW = w * 0.16f;
        const float ammoX = w - ammoBarW - w * 0.03f;
        const float ammoTextY = barY + barH * 0.45f;

        if (bottomTexts[0])
        {
            std::ostringstream oss;
            if (active.isInfiniteAmmo)
            {
                oss << "INFINITE";
                bottomTexts[0]->setFillColor(UITheme::AccentGreen);
            }
            else if (active.isReloading)
            {
                oss << "RELOADING...";
                bottomTexts[0]->setFillColor(UITheme::AccentOrange);
            }
            else
            {
                oss << active.currentAmmo << " / " << active.ammoCapacity;
                const float ratio = active.ammoCapacity > 0
                    ? static_cast<float>(active.currentAmmo) / static_cast<float>(active.ammoCapacity) : 0.0f;
                bottomTexts[0]->setFillColor(ratio < 0.25f ? UITheme::AccentRed : UITheme::TextWhite);
            }
            bottomTexts[0]->setString(oss.str());
            const sf::FloatRect ab = bottomTexts[0]->getLocalBounds();
            bottomTexts[0]->setOrigin({ ab.position.x + ab.size.x, ab.position.y + ab.size.y * 0.5f });
            bottomTexts[0]->setPosition({ ammoX + ammoBarW, ammoTextY });
            bottomTexts[0]->setCharacterSize(static_cast<unsigned int>(barH * 0.22f));
        }

        // 弹药条
        const float ammoBarH = barH * 0.15f;
        const float ammoBarY = barY + barH * 0.6f;
        ammoBarBg.setSize({ ammoBarW, ammoBarH });
        ammoBarBg.setPosition({ ammoX, ammoBarY });
        ammoBarBg.setFillColor(UITheme::AmmoBg);
        ammoBarBg.setOutlineColor(UITheme::PanelBorder);
        ammoBarBg.setOutlineThickness(1.0f);

        float ratio = 1.0f;
        if (!active.isInfiniteAmmo && active.ammoCapacity > 0)
            ratio = static_cast<float>(active.currentAmmo) / static_cast<float>(active.ammoCapacity);
        if (active.isReloading) ratio = 0.0f;

        const float fillW = (ammoBarW - 4.0f) * std::clamp(ratio, 0.0f, 1.0f);
        ammoBarFill.setSize({ fillW, ammoBarH - 4.0f });
        ammoBarFill.setPosition({ ammoX + 2.0f, ammoBarY + 2.0f });
        ammoBarFill.setFillColor(ratio < 0.25f ? UITheme::AmmoLow : UITheme::AmmoFill);

        if (bottomTexts[1])
        {
            bottomTexts[1]->setString(active.fullName);
            bottomTexts[1]->setCharacterSize(static_cast<unsigned int>(barH * 0.2f));
            const sf::FloatRect nb = bottomTexts[1]->getLocalBounds();
            bottomTexts[1]->setOrigin({ nb.position.x + nb.size.x * 0.5f, nb.position.y + nb.size.y });
            bottomTexts[1]->setPosition({ ammoX + ammoBarW * 0.5f, ammoTextY - 2.0f });
        }
    }

    // 图形元素
    sf::RectangleShape topBarBg;
    sf::RectangleShape modeIndicator;
    sf::RectangleShape bottomBarBg;
    sf::RectangleShape bottomAccentLine;
    struct WeaponSlot { sf::RectangleShape bg; };
    std::array<WeaponSlot, 3> weaponSlots;
    sf::RectangleShape ammoBarBg;
    sf::RectangleShape ammoBarFill;

    // 文字元素（optional 因为 SFML 3.1 无默认构造）
    std::optional<sf::Text> modeNameText;
    std::optional<sf::Text> timerText;
    std::array<std::optional<sf::Text>, 4> topRightTexts;
    std::array<std::optional<sf::Text>, 3> weaponSlotLabels;
    std::array<std::optional<sf::Text>, 3> weaponKeyHints;
    std::array<std::optional<sf::Text>, 2> bottomTexts;

    bool needsTextInit = false;
};
