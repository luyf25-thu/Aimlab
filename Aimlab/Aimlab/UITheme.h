#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

// ============================================================
// UITheme — 集中管理 UI 配色、尺寸、字体
// 高对比度竞技风格（AimLab 风格）
// ============================================================
namespace UITheme
{
    // ── 主色调 ──────────────────────────────────
    inline const sf::Color BgDark{ 18, 18, 30, 255 };          // 最深底色
    inline const sf::Color PanelBg{ 22, 22, 42, 220 };         // 面板背景（半透明）
    inline const sf::Color PanelBorder{ 60, 70, 100, 180 };    // 面板边框
    inline const sf::Color OverlayBg{ 0, 0, 0, 175 };          // 遮罩层

    // ── 强调色 ──────────────────────────────────
    inline const sf::Color AccentBlue{ 79, 195, 247, 255 };    // 主强调色 #4FC3F7
    inline const sf::Color AccentOrange{ 255, 152, 0, 255 };   // 次强调色 #FF9800
    inline const sf::Color AccentGreen{ 76, 175, 80, 255 };    // 成功/命中 #4CAF50
    inline const sf::Color AccentRed{ 244, 67, 54, 255 };      // 危险/失误 #F44336

    // ── 文字颜色 ────────────────────────────────
    inline const sf::Color TextWhite{ 245, 245, 250, 255 };    // 主文字
    inline const sf::Color TextGray{ 160, 170, 190, 255 };     // 次要文字
    inline const sf::Color TextDim{ 100, 108, 130, 255 };      // 暗淡文字

    // ── 武器栏颜色 ──────────────────────────────
    inline const sf::Color WeaponBg{ 28, 28, 50, 200 };
    inline const sf::Color WeaponActive{ 79, 195, 247, 60 };
    inline const sf::Color WeaponInactive{ 35, 35, 55, 180 };

    // ── 弹药条颜色 ──────────────────────────────
    inline const sf::Color AmmoBg{ 35, 35, 55, 255 };
    inline const sf::Color AmmoFill{ 79, 195, 247, 255 };
    inline const sf::Color AmmoLow{ 244, 67, 54, 255 };

    // ── 按钮颜色 ────────────────────────────────
    inline const sf::Color BtnNormal{ 45, 50, 75, 230 };
    inline const sf::Color BtnHover{ 60, 68, 100, 240 };
    inline const sf::Color BtnPressed{ 35, 40, 60, 250 };
    inline const sf::Color BtnBorder{ 79, 195, 247, 100 };
    inline const sf::Color BtnBorderHover{ 79, 195, 247, 220 };

    // ── 尺寸常量 ────────────────────────────────
    inline constexpr float PanelPadding = 14.0f;
    inline constexpr float PanelBorderWidth = 1.5f;
    inline constexpr float ButtonHeight = 46.0f;
    inline constexpr float ButtonMinWidth = 220.0f;
    inline constexpr float TopBarHeight = 44.0f;
    inline constexpr float BottomBarHeight = 80.0f;
    inline constexpr float WeaponSlotWidth = 80.0f;
    inline constexpr float WeaponSlotHeight = 64.0f;
    inline constexpr float AmmoBarWidth = 160.0f;
    inline constexpr float AmmoBarHeight = 12.0f;

    // ── 全局字体指针（Game 初始化时设置）───────
    inline sf::Font* DefaultFont = nullptr;
}
