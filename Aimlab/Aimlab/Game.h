#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include <filesystem>
#include <windows.h>
#include <algorithm>
#include <sstream>
#include "USP.h"
#include "Ak47.h"
#include "M4A1.h"
#include "TargetSpawner.h"
#include "Crosshair.h"
#include "ScoreManager.h"

// 游戏主控类：驱动所有子系统
class Game
{
public:
    // 初始化窗口与系统组件
    Game()
        : window(sf::VideoMode({ 800, 600 }), "Aimlab OOP Demo"),
          targetPool(60),
          spawner(&targetPool, 0.9f)
    {
        window.setFramerateLimit(144);
        view = window.getDefaultView();
        window.setView(view);
        // 加载背景贴图（固定在屏幕上）
        const std::filesystem::path backgroundPath = getRepoRootDir() / "assets" / "background.jpg";
        if (backgroundTexture.loadFromFile(backgroundPath.u8string()))
        {
            backgroundSprite.emplace(backgroundTexture);
        }
        const std::filesystem::path fontPath = getRepoRootDir() / "assets" / "font.ttf";
        if (uiFont.openFromFile(fontPath.u8string()))
        {
            ammoText.emplace(uiFont);
            ammoText->setCharacterSize(20);
            ammoText->setFillColor(sf::Color::White);
        }
        activeWeapon = &usp;
        crosshair.setWeapon(activeWeapon);
        window.setMouseCursorVisible(false);
        const sf::Vector2i center = getWindowCenter();
        sf::Mouse::setPosition(center, window);
    }

    // 运行主循环
    void run()
    {
        sf::Clock clock;
        while (window.isOpen())
        {
            processEvents();
            const float deltaTime = clock.restart().asSeconds();
            update(deltaTime);
            render();
        }
    }

private:
    // 处理窗口与输入事件
    void processEvents()
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                    continue;
                }
                if (key->code == sf::Keyboard::Key::R)
                {
                    activeWeapon->reload();
                    continue;
                }
                handleWeaponSwitch(key->code);
            }
            else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (pressed->button == sf::Mouse::Button::Left)
                {
                    isFiring = true;
                    tryFireOnce();
                }
            }
            else if (const auto* released = event->getIf<sf::Event::MouseButtonReleased>())
            {
                if (released->button == sf::Mouse::Button::Left)
                {
                    isFiring = false;
                }
            }
            else if (event->is<sf::Event::MouseMoved>())
            {
                handleMouseLook();
            }
        }
    }

    // 切换武器
    void handleWeaponSwitch(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Key::Num1)
        {
            activeWeapon = &usp;
        }
        else if (key == sf::Keyboard::Key::Num2)
        {
            activeWeapon = &ak47;
        }
        else if (key == sf::Keyboard::Key::Num3)
        {
            activeWeapon = &m4a1;
        }
        crosshair.setWeapon(activeWeapon);
    }

    // 更新游戏逻辑
    void update(float deltaTime)
    {
        activeWeapon->update(deltaTime);
        spawner.update(deltaTime, window.getSize());

        for (const auto& target : targetPool.getTargets())
        {
            target->update(deltaTime);
        }

        if (isFiring && activeWeapon->isAutomatic())
        {
            tryFireOnce();
        }

        crosshair.updatePosition(getWindowCenterFloat());
        updateAmmoText();
    }

    // 尝试开火并处理命中判定
    void tryFireOnce()
    {
        if (!activeWeapon->fire())
        {
            return;
        }

        const sf::View recoilView = getRecoilView();
        const sf::Vector2i centerPixel = getWindowCenter();
        const sf::Vector2f shotPosition = window.mapPixelToCoords(centerPixel, recoilView);
        bool hit = false;
        for (const auto& target : targetPool.getTargets())
        {
            if (target->getIsActive() && target->isHit(shotPosition))
            {
                target->onHit();
                hit = true;
                break;
            }
        }

        if (hit)
        {
            scoreManager.recordHit();
        }
        else
        {
            scoreManager.recordMiss();
        }
    }

    // 渲染所有内容
    void render()
    {
        window.clear(sf::Color(20, 20, 20));
        const sf::View recoilView = getRecoilView();
        window.setView(recoilView);
        if (backgroundSprite)
        {
            window.draw(*backgroundSprite);
        }
        for (const auto& target : targetPool.getTargets())
        {
            target->render(window);
        }
        window.setView(window.getDefaultView());
        if (ammoText)
        {
            window.draw(*ammoText);
        }
        crosshair.render(window);
        window.display();
    }

    // 获取窗口中心（整数）
    sf::Vector2i getWindowCenter() const
    {
        const sf::Vector2u size = window.getSize();
        return { static_cast<int>(size.x / 2), static_cast<int>(size.y / 2) };
    }

    // 获取窗口中心（浮点）
    sf::Vector2f getWindowCenterFloat() const
    {
        const sf::Vector2u size = window.getSize();
        return { static_cast<float>(size.x) * 0.5f, static_cast<float>(size.y) * 0.5f };
    }

    // 鼠标控制视角平移并锁定中心点
    void handleMouseLook()
    {
        const sf::Vector2i center = getWindowCenter();
        const sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        const sf::Vector2i delta = mousePos - center;
        if (delta.x != 0 || delta.y != 0)
        {
            const sf::Vector2f offset(static_cast<float>(delta.x) * viewMoveSpeed,
                                      static_cast<float>(delta.y) * viewMoveSpeed);
            const sf::Vector2f newCenter = clampViewCenter(view.getCenter() + offset);
            view.setCenter(newCenter);
            window.setView(view);
        }
        sf::Mouse::setPosition(center, window);
    }

    // 计算带后坐力抖动的视图
    sf::View getRecoilView() const
    {
        sf::View recoilView = view;
        if (activeWeapon)
        {
            recoilView.move(activeWeapon->getRecoilOffset());
        }
        return recoilView;
    }

    // 获取可执行文件所在目录
    static std::filesystem::path getExecutableDir()
    {
        wchar_t buffer[MAX_PATH] = {};
        const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (length == 0)
        {
            return std::filesystem::current_path();
        }
        return std::filesystem::path(buffer).parent_path();
    }

    // 获取仓库根目录（从可执行文件目录向上回退）
    static std::filesystem::path getRepoRootDir()
    {
        return getExecutableDir().parent_path().parent_path().parent_path();
    }

    // 限制视图中心在背景范围内
    sf::Vector2f clampViewCenter(const sf::Vector2f& center) const
    {
        if (backgroundTexture.getSize().x == 0 || backgroundTexture.getSize().y == 0)
        {
            return center;
        }

        const sf::Vector2f viewSize = view.getSize();
        const sf::Vector2u bgSize = backgroundTexture.getSize();
        const float halfWidth = viewSize.x * 0.5f;
        const float halfHeight = viewSize.y * 0.5f;

        const float minX = halfWidth;
        const float maxX = std::max(halfWidth, static_cast<float>(bgSize.x) - halfWidth);
        const float minY = halfHeight;
        const float maxY = std::max(halfHeight, static_cast<float>(bgSize.y) - halfHeight);

        return {
            std::clamp(center.x, minX, maxX),
            std::clamp(center.y, minY, maxY)
        };
    }

    // 更新右下角弹药显示
    void updateAmmoText()
    {
        if (!ammoText || !activeWeapon)
        {
            return;
        }

        std::ostringstream text;
        if (activeWeapon->getIsReloading())
        {
            text << "Reloading...";
        }
        else
        {
            text << activeWeapon->getCurrentAmmo() << " / " << activeWeapon->getAmmoCapacity();
        }
        ammoText->setString(text.str());

        const sf::Vector2u size = window.getSize();
        const sf::FloatRect bounds = ammoText->getLocalBounds();
        ammoText->setOrigin({ bounds.size.x, bounds.size.y });
        ammoText->setPosition({ static_cast<float>(size.x) - 20.0f, static_cast<float>(size.y) - 20.0f });
    }

    sf::RenderWindow window;
    Usp usp;
    Ak47 ak47;
    M4A1 m4a1;
    Weapon* activeWeapon = nullptr;
    TargetPool targetPool;
    TargetSpawner spawner;
    Crosshair crosshair;
    ScoreManager scoreManager;
    bool isFiring = false;
    sf::View view;
    float viewMoveSpeed = 0.35f;
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    sf::Font uiFont;
    std::optional<sf::Text> ammoText;
};
