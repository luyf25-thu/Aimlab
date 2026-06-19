#pragma once

#include <SFML/Graphics.hpp>
#include <filesystem>
#include "GameState.h"
#include "MainMenu.h"
#include "GameHUD.h"
#include "PauseAndResult.h"
#include "HelpScreen.h"

// ============================================================
// UIManager — 管理所有 UI 屏幕，路由事件与渲染
// ============================================================
class UIManager
{
public:
    UIManager() = default;

    // 初始化所有屏幕
    bool init(const std::filesystem::path& bgPath, const sf::Vector2u& windowSize)
    {
        // 设置全局字体引用（由 Game 在加载字体后设置）
        if (!UITheme::DefaultFont)
        {
            return false;
        }

        // ── 设置回调（必须在 init 之前，因为 init 里创建的按钮要用）──
        mainMenu.setStateCallback([this](GameState s) { requestStateChange(s); });
        pauseOverlay.setStateCallback([this](GameState s) { requestStateChange(s); });
        resultScreen.setStateCallback([this](GameState s) { requestStateChange(s); });
        helpScreen.setStateCallback([this](GameState s) { requestStateChange(s); });

        // ── 各屏幕延迟初始化（依赖字体）───
        hud.initTexts();
        pauseOverlay.init();
        resultScreen.init();
        helpScreen.init();

        // ── 主菜单（加载背景）──────────────
        if (!mainMenu.init(bgPath))
        {
            return false;
        }
        mainMenu.onResize(windowSize);

        // ── 其他屏幕调整尺寸 ────────────────
        pauseOverlay.onResize(windowSize);
        resultScreen.onResize(windowSize);
        helpScreen.onResize(windowSize);

        currentState = GameState::MainMenu;
        return true;
    }

    // ── 状态查询 ──────────────────────────────
    GameState getState() const
    {
        return currentState;
    }

    bool isPendingStateChange() const
    {
        return pendingState.has_value();
    }

    GameState consumePendingState()
    {
        const GameState s = pendingState.value_or(currentState);
        applyState(s);
        pendingState.reset();
        return s;
    }

    void setState(GameState state)
    {
        applyState(state);
    }

    // ── 窗口大小改变 ──────────────────────────
    void onResize(const sf::Vector2u& windowSize)
    {
        mainMenu.onResize(windowSize);
        pauseOverlay.onResize(windowSize);
        resultScreen.onResize(windowSize);
        helpScreen.onResize(windowSize);
    }

    // ── 事件分发 ──────────────────────────────
    void handleEvent(const sf::Event& event)
    {
        switch (currentState)
        {
        case GameState::MainMenu:
            mainMenu.handleEvent(event);
            break;
        case GameState::Playing:
            // 游戏中事件由 Game 自己处理
            break;
        case GameState::Paused:
            pauseOverlay.handleEvent(event);
            break;
        case GameState::Result:
            resultScreen.handleEvent(event);
            break;
        case GameState::Help:
            helpScreen.handleEvent(event);
            break;
        case GameState::Exit:
            break;
        }
    }

    // ── 渲染 ──────────────────────────────────
    void render(sf::RenderWindow& window)
    {
        switch (currentState)
        {
        case GameState::MainMenu:
            mainMenu.render(window);
            break;
        case GameState::Playing:
            // HUD 由 Game 在游戏渲染流程中调用
            break;
        case GameState::Paused:
            // 游戏画面已在 Game 中渲染，只需叠加暂停 UI
            pauseOverlay.render(window);
            break;
        case GameState::Result:
            resultScreen.render(window);
            break;
        case GameState::Help:
            helpScreen.render(window);
            break;
        case GameState::Exit:
            break;
        }
    }

    // ── 数据更新接口（供 Game 调用）───────────
    void updateResultStats(int hits, int misses, int totalShots,
                           float accuracy, float avgTime, float totalTime)
    {
        resultScreen.setStats(hits, misses, totalShots, accuracy, avgTime, totalTime);
    }

    // 设置回调
    void setPauseWeapon(int idx) { pauseOverlay.setActiveWeapon(idx); }

    void setPauseRestartCallback(std::function<void()> cb)
    {
        pauseOverlay.setRestartCallback(std::move(cb));
    }

    void setResultRestartCallback(std::function<void()> cb)
    {
        resultScreen.setRestartCallback(std::move(cb));
    }

    // ── GameHUD 对外暴露（供 Game 渲染）───────
    GameHUD hud;

private:
    void requestStateChange(GameState state)
    {
        pendingState = state;
    }

    void applyState(GameState state)
    {
        currentState = state;
        pendingState.reset();
    }

    GameState currentState = GameState::MainMenu;
    std::optional<GameState> pendingState;

    MainMenu mainMenu;
    PauseOverlay pauseOverlay;
    ResultScreen resultScreen;
    HelpScreen helpScreen;
};
