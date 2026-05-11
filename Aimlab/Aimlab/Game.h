#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
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
        for (const auto& target : targetPool.getTargets())
        {
            target->render(window);
        }
        window.setView(window.getDefaultView());
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
            view.move(offset);
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
};
