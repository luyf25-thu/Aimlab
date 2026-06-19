#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <optional>
#include <filesystem>
#include <windows.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <array>
#include "USP.h"
#include "Ak47.h"
#include "M4A1.h"
#include "TargetSpawner.h"
#include "SimpleMode.h"
#include "HardMode.h"
#include "RecoilMode.h"
#include "Crosshair.h"
#include "ScoreManager.h"
#include "UITheme.h"
#include "UIComponents.h"
#include "GameState.h"
#include "GameHUD.h"
#include "UIManager.h"

// 游戏主控类：驱动所有子系统
class Game
{
public:
    // 初始化窗口与系统组件
    Game()
        : window(sf::VideoMode({ 1024, 768 }), "Aimlab OOP Demo"),
          targetPool(60),
          spawner(&targetPool, &simpleMode, 0.0f)
    {
        window.setFramerateLimit(144);
        uiView = sf::View(sf::FloatRect({0.f, 0.f}, {1024.f, 768.f}));

        // 加载背景贴图（位于 exe 同目录）
        const std::filesystem::path backgroundPath = getExecutableDir() / "background.jpg";
        if (backgroundTexture.loadFromFile(backgroundPath.u8string()))
        {
            backgroundSprite.emplace(backgroundTexture);
            bgSize = backgroundTexture.getSize();
            // 游戏视图：显示与窗口等大的世界区域（1:1像素映射）
            view = sf::View(sf::FloatRect(
                { static_cast<float>(bgSize.x - 1024) * 0.5f,
                  static_cast<float>(bgSize.y - 768) * 0.5f },
                { 1024.f, 768.f }));
        }
        else
        {
            view = sf::View(sf::FloatRect({0.f, 0.f}, {1024.f, 768.f}));
        }
        window.setView(view);

        // 加载字体（位于 exe 同目录）
        const std::filesystem::path fontPath = getExecutableDir() / "font.ttf";
        if (uiFont.openFromFile(fontPath.u8string()))
        {
            // 设置全局字体供所有 UI 组件使用
            UITheme::DefaultFont = &uiFont;

            // 初始化 UI 管理器
            if (uiManager.init(backgroundPath, window.getSize()))
            {
                // 设置暂停回调：重新开始
                uiManager.setPauseRestartCallback([this]() {
                    restartGame();
                });
                // 设置结算回调：重新开始
                uiManager.setResultRestartCallback([this]() {
                    restartGame();
                });
            }

            // 保留旧的简单文字（GameHUD 内部自行管理文字）
            // ammoText / statsText 不再需要，由 GameHUD 替代
        }

        activeWeapon = &usp;
        currentMode = &simpleMode;
        activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);

        // 初始缩放：背景、靶子半径、网格间距
        updateWorldScale();

        // 初始状态：主菜单（鼠标可见）
        gameState = GameState::MainMenu;
        window.setMouseCursorVisible(true);
    }

    // 运行主循环
    void run()
    {
        sf::Clock clock;
        while (window.isOpen())
        {
            // 处理待定的状态切换
            if (uiManager.isPendingStateChange())
            {
                const GameState newState = uiManager.consumePendingState();
                handleStateTransition(newState);
            }

            processEvents();
            const float deltaTime = clock.restart().asSeconds();

            // 仅在游戏进行中更新游戏逻辑
            if (gameState == GameState::Playing)
            {
                update(deltaTime);
            }

            render();

            // 检查退出
            if (gameState == GameState::Exit)
            {
                window.close();
            }
        }
    }

private:
    // ── 状态切换处理 ──────────────────────────
    void handleStateTransition(GameState newState)
    {
        if (newState == GameState::Exit)
        {
            gameState = GameState::Exit;
            return;
        }

        if (newState == GameState::Playing && gameState != GameState::Playing)
        {
            // 进入游戏：显示/隐藏鼠标
            if (gameState == GameState::MainMenu || gameState == GameState::Result)
            {
                restartGame();
            }
            window.setMouseCursorVisible(false);
            const sf::Vector2i center = getWindowCenter();
            sf::Mouse::setPosition(center, window);
        }
        else if (newState == GameState::MainMenu)
        {
            window.setMouseCursorVisible(true);
        }
        else if (newState == GameState::Paused)
        {
            window.setMouseCursorVisible(true);
            // 更新暂停界面的武器高亮
            int widx = (activeWeapon == &ak47) ? 1 : (activeWeapon == &m4a1) ? 2 : 0;
            uiManager.setPauseWeapon(widx);
        }
        else if (newState == GameState::Result)
        {
            window.setMouseCursorVisible(true);
            // 填充结算数据
            const float accuracy = scoreManager.getAccuracy();
            const float avgTime = scoreManager.getHits() > 0
                ? elapsedTime / static_cast<float>(scoreManager.getHits()) : 0.0f;
            uiManager.updateResultStats(
                scoreManager.getHits(),
                scoreManager.getMisses(),
                scoreManager.getHits() + scoreManager.getMisses(),
                accuracy, avgTime, elapsedTime);
        }
        else if (newState == GameState::Help)
        {
            window.setMouseCursorVisible(true);
        }

        gameState = newState;
        uiManager.setState(newState);  // 同步 UIManager 状态
    }

    // 重新开始游戏
    void restartGame()
    {
        targetPool.deactivateAll();
        scoreManager.reset();
        elapsedTime = 0.0f;
        avgSecondsPerHit = 0.0f;
        activeWeapon = &usp;
        activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
        modeIndex = 0;
        currentMode = &simpleMode;
        spawner.setMode(currentMode);
        if (currentMode) currentMode->reset();
        infiniteAmmoEnabled = true;
        remainingTime = roundTimeLimit;
        // 重置视图：居中于游戏世界
        const sf::Vector2u ws = window.getSize();
        view.setSize({ static_cast<float>(ws.x), static_cast<float>(ws.y) });
        const sf::Vector2u area = getSpawnAreaSize();
        view.setCenter(clampViewCenter({
            static_cast<float>(area.x) * 0.5f,
            static_cast<float>(area.y) * 0.5f
        }));
        window.setView(view);
    }

    // 进入结算界面
    void enterResultScreen()
    {
        handleStateTransition(GameState::Result);
    }

    // 处理窗口与输入事件
    void processEvents()
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                return;
            }

            // 窗口大小变化：保持 1:1 像素映射，扩大视野
            if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                const sf::Vector2u ns = resized->size;
                const sf::Vector2f oldCenter = view.getCenter();
                view.setSize({ static_cast<float>(ns.x), static_cast<float>(ns.y) });
                view.setCenter(clampViewCenter(oldCenter));
                window.setView(view);
                // UI 视图同步更新
                uiView = sf::View(sf::FloatRect({0.f, 0.f}, { static_cast<float>(ns.x), static_cast<float>(ns.y) }));
                uiManager.onResize(ns);
                // 更新靶子大小缩放 + 网格间距 + 背景缩放
                updateWorldScale();
            }

            // 非游戏状态：事件交给 UI 管理器
            if (gameState != GameState::Playing)
            {
                uiManager.handleEvent(*event);

                // 在 MainMenu 直接检测按钮点击（比例与 MainMenu::layoutElements 一致）
                if (gameState == GameState::MainMenu)
                {
                    if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (pressed->button == sf::Mouse::Button::Left)
                        {
                            const int mx = pressed->position.x;
                            const int my = pressed->position.y;
                            const int w = static_cast<int>(window.getSize().x);
                            const int h = static_cast<int>(window.getSize().y);
                            const int cx = w / 2;
                            const int cy = h / 2;
                            const int bw = static_cast<int>(w * 0.28f);
                            const int bh = static_cast<int>(h * 0.065f);
                            const int bx = cx - bw / 2;

                            const int btn1Y = cy - static_cast<int>(h * 0.02f);
                            const int btn2Y = cy + static_cast<int>(h * 0.06f);
                            const int btn3Y = cy + static_cast<int>(h * 0.14f);

                            if (mx >= bx && mx <= bx + bw && my >= btn1Y && my <= btn1Y + bh)
                                handleStateTransition(GameState::Playing);
                            else if (mx >= bx && mx <= bx + bw && my >= btn2Y && my <= btn2Y + bh)
                                handleStateTransition(GameState::Help);
                            else if (mx >= bx && mx <= bx + bw && my >= btn3Y && my <= btn3Y + bh)
                                { gameState = GameState::Exit; return; }
                        }
                    }
                }

                // Paused 界面直接检测按钮点击
                if (gameState == GameState::Paused)
                {
                    if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>())
                    {
                        if (pressed->button == sf::Mouse::Button::Left)
                        {
                            const int mx = pressed->position.x;
                            const int my = pressed->position.y;
                            const int w = static_cast<int>(window.getSize().x);
                            const int h = static_cast<int>(window.getSize().y);
                            const int cx = w / 2;
                            const int cy = h / 2;
                            const int bw = static_cast<int>(w * 0.28f);
                            const int bh = static_cast<int>(h * 0.065f);
                            const int bx = cx - bw / 2;

                            // 武器切换按钮
                            const int wbw = static_cast<int>(w * 0.09f);
                            const int wbh = static_cast<int>(h * 0.06f);
                            const int wby = cy - static_cast<int>(h * 0.09f);
                            const int wbStartX = cx - static_cast<int>(w * 0.14f);
                            bool weaponClicked = false;
                            for (int i = 0; i < 3; ++i)
                            {
                                const int wx = wbStartX + i * (wbw + static_cast<int>(w * 0.008f));
                                if (mx >= wx && mx <= wx + wbw && my >= wby && my <= wby + wbh)
                                {
                                    if (i == 0) activeWeapon = &usp;
                                    else if (i == 1) activeWeapon = &ak47;
                                    else activeWeapon = &m4a1;
                                    activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
                                    uiManager.setPauseWeapon(i);
                                    weaponClicked = true;
                                    break;
                                }
                            }

                            if (!weaponClicked)
                            {
                                const int r1Y = cy - static_cast<int>(h * 0.02f);
                                const int r2Y = cy + static_cast<int>(h * 0.065f);
                                const int r3Y = cy + static_cast<int>(h * 0.15f);
                                if (mx >= bx && mx <= bx + bw && my >= r1Y && my <= r1Y + bh)
                                    handleStateTransition(GameState::Playing);
                                else if (mx >= bx && mx <= bx + bw && my >= r2Y && my <= r2Y + bh)
                                    { restartGame(); handleStateTransition(GameState::Playing); }
                                else if (mx >= bx && mx <= bx + bw && my >= r3Y && my <= r3Y + bh)
                                    handleStateTransition(GameState::MainMenu);
                            }
                        }
                    }
                }

                // 按键处理
                if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                {
                    if (key->code == sf::Keyboard::Key::Escape)
                    {
                        if (gameState == GameState::MainMenu)
                        {
                            gameState = GameState::Exit;
                            return;
                        }
                        else if (gameState == GameState::Paused)
                        {
                            handleStateTransition(GameState::Playing);
                        }
                        else if (gameState == GameState::Help)
                        {
                            handleStateTransition(GameState::MainMenu);
                        }
                    }
                }
                continue;
            }

            // ── 以下为游戏进行中的事件处理 ──────
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Space)
                {
                    handleStateTransition(GameState::Paused);
                    continue;
                }
                if (key->code == sf::Keyboard::Key::M)
                {
                    toggleMode();
                    continue;
                }
                if (key->code == sf::Keyboard::Key::X)
                {
                    infiniteAmmoEnabled = !infiniteAmmoEnabled;
                    activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
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
                    // 先检查是否点击了底部武器栏（比例化）
                    const int mx = pressed->position.x;
                    const int my = pressed->position.y;
                    const int winW = static_cast<int>(window.getSize().x);
                    const int winH = static_cast<int>(window.getSize().y);
                    const int slotH = static_cast<int>(winH * 0.09f);
                    const int slotW = static_cast<int>(winW * 0.08f);
                    const int gap = static_cast<int>(winW * 0.008f);
                    const int slotY = winH - static_cast<int>(winH * 0.11f);

                    bool clickedSlot = false;
                    for (int i = 0; i < 3; ++i)
                    {
                        const int sx = static_cast<int>(winW * 0.02f) + i * (slotW + gap);
                        if (mx >= sx && mx <= sx + slotW && my >= slotY && my <= slotY + slotH)
                        {
                            // 点击武器槽切换武器
                            if (i == 0) activeWeapon = &usp;
                            else if (i == 1) activeWeapon = &ak47;
                            else activeWeapon = &m4a1;
                            activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
                            clickedSlot = true;
                            break;
                        }
                    }

                    if (!clickedSlot)
                    {
                        isFiring = true;
                        tryFireOnce();
                    }
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
        activeWeapon->setInfiniteAmmo(infiniteAmmoEnabled);
    }

    // 更新游戏逻辑
    void update(float deltaTime)
    {
        elapsedTime += deltaTime;
        remainingTime -= deltaTime;

        // 时间到，进入结算
        if (remainingTime <= 0.0f)
        {
            remainingTime = 0.0f;
            enterResultScreen();
            return;
        }

        activeWeapon->update(deltaTime, isFiring);
        spawner.update(deltaTime, getSpawnAreaSize());

        for (const auto& target : targetPool.getTargets())
        {
            target->update(deltaTime);
        }

        if (isFiring && activeWeapon->isAutomatic())
        {
            tryFireOnce();
        }

        crosshair.updatePosition(getWindowCenterFloat());

        // 更新 HUD 数据
        updateHUD();
    }

    // 更新 HUD
    void updateHUD()
    {
        // 武器信息
        GameHUD::WeaponInfo activeInfo = getWeaponInfo(activeWeapon, true);
        std::array<GameHUD::WeaponInfo, 3> allWeapons = {
            getWeaponInfo(&usp, activeWeapon == &usp),
            getWeaponInfo(&ak47, activeWeapon == &ak47),
            getWeaponInfo(&m4a1, activeWeapon == &m4a1)
        };

        // 统计信息
        GameHUD::StatsInfo stats;
        stats.hits = scoreManager.getHits();
        stats.misses = scoreManager.getMisses();
        stats.totalShots = stats.hits + stats.misses;
        stats.accuracy = scoreManager.getAccuracy();
        stats.avgSecondsPerHit = (stats.hits > 0)
            ? elapsedTime / static_cast<float>(stats.hits) : 0.0f;

        // 模式信息
        GameHUD::ModeInfo modeInfo;
        switch (modeIndex)
        {
        case 0: modeInfo.name = "Simple"; break;
        case 1: modeInfo.name = "Hard"; break;
        case 2: modeInfo.name = "Recoil"; break;
        }
        modeInfo.index = modeIndex;
        modeInfo.totalModes = 3;

        uiManager.hud.update(activeInfo, allWeapons, stats, modeInfo, remainingTime, window.getSize());
    }

    // 从 Weapon* 提取显示信息
    GameHUD::WeaponInfo getWeaponInfo(Weapon* weapon, bool active) const
    {
        GameHUD::WeaponInfo info;
        info.isActive = active;
        info.currentAmmo = weapon->getCurrentAmmo();
        info.ammoCapacity = weapon->getAmmoCapacity();
        info.isReloading = weapon->getIsReloading();
        info.isInfiniteAmmo = weapon->getInfiniteAmmo();
        info.fireType = weapon->isAutomatic() ? "Auto" : "Semi";

        if (weapon == &usp)
        {
            info.shortName = "USP";
            info.fullName = "Pistol";
            info.keyIndex = 1;
        }
        else if (weapon == &ak47)
        {
            info.shortName = "AK-47";
            info.fullName = "Assault Rifle";
            info.keyIndex = 2;
        }
        else
        {
            info.shortName = "M4A1";
            info.fullName = "Carbine";
            info.keyIndex = 3;
        }

        return info;
    }

    // 获取生成区域尺寸（确保始终比视野大，留有平移空间）
    sf::Vector2u getSpawnAreaSize() const
    {
        const unsigned int vw = window.getSize().x;
        const unsigned int vh = window.getSize().y;
        // 游戏世界至少是背景大小，且至少是视野的 1.5 倍，确保全屏下也有足够平移范围
        const unsigned int areaW = std::max({ bgSize.x, vw, vw * 3 / 2 });
        const unsigned int areaH = std::max({ bgSize.y, vh, vh * 3 / 2 });
        return { areaW, areaH };
    }

    // 分辨率缩放因子（基于 768p 基准）
    float getResolutionScale() const
    {
        return static_cast<float>(window.getSize().y) / 768.0f;
    }

    // 同步世界缩放：背景图、靶子半径、网格间距
    void updateWorldScale()
    {
        const float resScale = getResolutionScale();
        spawner.setRadiusScale(resScale);
        spawner.setSpacingScale(resScale);

        // 缩放背景图以填满生成区域（消除黑边）
        if (backgroundSprite && bgSize.x > 0 && bgSize.y > 0)
        {
            const sf::Vector2u area = getSpawnAreaSize();
            const sf::Vector2u texSize = backgroundTexture.getSize();
            const float scaleX = static_cast<float>(area.x) / static_cast<float>(texSize.x);
            const float scaleY = static_cast<float>(area.y) / static_cast<float>(texSize.y);
            // 等比缩放，确保覆盖整个区域
            const float scale = std::max(scaleX, scaleY);
            backgroundSprite->setScale({ scale, scale });
            // 居中放置
            const float sw = static_cast<float>(texSize.x) * scale;
            const float sh = static_cast<float>(texSize.y) * scale;
            backgroundSprite->setPosition({
                (static_cast<float>(area.x) - sw) * 0.5f,
                (static_cast<float>(area.y) - sh) * 0.5f
            });
        }
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
                const bool destroyed = target->onHit();
                if (destroyed && currentMode)
                {
                    currentMode->onTargetHit(target->getPosition());
                }
                hit = true;
                break;
            }
        }

        if (hit)
        {
            scoreManager.recordHit();
            const int hits = scoreManager.getHits();
            if (hits > 0)
            {
                avgSecondsPerHit = elapsedTime / static_cast<float>(hits);
            }
        }
        else
        {
            scoreManager.recordMiss();
        }
    }

    // ── 渲染所有内容 ──────────────────────────
    void render()
    {
        // MainMenu 和 Help 界面：直接渲染 UI
        if (gameState == GameState::MainMenu || gameState == GameState::Help)
        {
            window.clear(sf::Color(20, 20, 20));
            window.setView(uiView);
            uiManager.render(window);
            window.display();
            return;
        }

        if (gameState == GameState::Result)
        {
            // 结算界面：游戏画面在背景静止
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
            window.setView(uiView);
            uiManager.render(window);
            window.display();
            return;
        }

        // Playing / Paused
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

        // 切回屏幕坐标绘制 HUD
        window.setView(uiView);

        // 绘制 HUD（GameHUD）
        uiManager.hud.render(window);

        // 绘制准星
        crosshair.render(window);

        // 暂停遮罩
        if (gameState == GameState::Paused)
        {
            uiManager.render(window);
        }

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
            // 移动速度随分辨率缩放，全屏下保持相同手感
            const float speed = viewMoveSpeed * getResolutionScale();
            const sf::Vector2f offset(static_cast<float>(delta.x) * speed,
                                      static_cast<float>(delta.y) * speed);
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
        return getExecutableDir().parent_path().parent_path().parent_path().parent_path();
    }

    // 限制视图中心在游戏世界范围内（使用动态生成区域，确保全屏下也可平移）
    sf::Vector2f clampViewCenter(const sf::Vector2f& center) const
    {
        const sf::Vector2u area = getSpawnAreaSize();
        if (area.x == 0 || area.y == 0)
            return center;

        const float halfW = view.getSize().x * 0.5f;
        const float halfH = view.getSize().y * 0.5f;
        const float minX = halfW;
        const float maxX = std::max(halfW, static_cast<float>(area.x) - halfW);
        const float minY = halfH;
        const float maxY = std::max(halfH, static_cast<float>(area.y) - halfH);

        return { std::clamp(center.x, minX, maxX), std::clamp(center.y, minY, maxY) };
    }

    // 切换简单/困难/压枪模式
    void toggleMode()
    {
        modeIndex = (modeIndex + 1) % 3;
        if (modeIndex == 0)
        {
            currentMode = &simpleMode;
        }
        else if (modeIndex == 1)
        {
            currentMode = &hardMode;
        }
        else
        {
            currentMode = &recoilMode;
        }
        spawner.setMode(currentMode);
        if (currentMode)
        {
            currentMode->reset();
        }
        targetPool.deactivateAll();
        scoreManager.reset();
        elapsedTime = 0.0f;
        avgSecondsPerHit = 0.0f;
    }

    // ── 核心组件 ──────────────────────────────
    sf::RenderWindow window;
    Usp usp;
    Ak47 ak47;
    M4A1 m4a1;
    Weapon* activeWeapon = nullptr;
    TargetPool targetPool;
    TargetSpawner spawner;
    Crosshair crosshair;
    ScoreManager scoreManager;

    // ── UI 系统 ───────────────────────────────
    UIManager uiManager;
    GameState gameState = GameState::MainMenu;

    // ── 游戏状态 ──────────────────────────────
    bool isFiring = false;
    bool infiniteAmmoEnabled = true;
    float elapsedTime = 0.0f;
    float avgSecondsPerHit = 0.0f;
    float remainingTime = 60.0f;
    float roundTimeLimit = 60.0f;
    int modeIndex = 0;

    // ── 视图 ──────────────────────────────────
    sf::View view;
    sf::View uiView;   // UI 视图（随窗口缩放更新）
    float viewMoveSpeed = 0.35f;

    // ── 资源 ──────────────────────────────────
    sf::Texture backgroundTexture;
    std::optional<sf::Sprite> backgroundSprite;
    sf::Vector2u bgSize{ 0, 0 };
    sf::Font uiFont;

    // ── 模式 ──────────────────────────────────
    SimpleMode simpleMode;
    HardMode hardMode;
    RecoilMode recoilMode;
    GameMode* currentMode = nullptr;
};
