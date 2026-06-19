# Aim Trainer - FPS 练习游戏

面向对象程序设计课程大作业 —— 基于 SFML 3.1.0 的 2D FPS 瞄准训练游戏。

## 功能

- **三种游戏模式**: Simple（简单）/ Hard（困难）/ Recoil（压枪训练）
- **三种武器**: USP 手枪 / AK-47 突击步枪 / M4A1 卡宾枪，可点击切换
- **完整 UI 系统**: 主菜单、HUD、暂停界面、结算界面、帮助界面
- **限时训练**: 每轮 60 秒，结束时自动显示命中率、平均反应时间等数据
- **评级系统**: S/A/B/C/D 等级评定
- **全屏适配**: 所有 UI 元素和靶子按窗口大小等比例缩放
- **瞄准镜准心**: 绿色环形准心，始终居中

## 环境要求

- **Visual Studio 2026 (v18)** 或更新版本（工具集 v145）
- **Windows SDK 10.0**
- **C++20**

SFML 3.1.0 库文件已包含在项目中，无需额外安装。

## 构建

1. 用 Visual Studio 打开 `Aimlab-master/Aimlab/Aimlab.sln`
2. 确保配置为 **Debug | x64**
3. 按 `Ctrl+Shift+B` 编译，或 `F5` 直接运行

构建后，SFML DLL 和资源文件会自动复制到输出目录。

## 操作说明

| 按键 | 功能 |
|------|------|
| 鼠标移动 | 移动视角/瞄准 |
| 鼠标左键 | 开火 |
| 1 / 2 / 3 | 切换武器 (USP / AK-47 / M4A1) |
| 点击底部武器槽 | 切换武器 |
| M | 切换游戏模式 |
| X | 切换无限弹药 |
| R | 换弹 |
| 空格 | 暂停（暂停界面可切换武器） |
| ESC | 返回主菜单 / 退出 |

## 项目结构

```
Aimlab-master/
├── Aimlab/
│   ├── Aimlab/          # 源代码（header-only C++20）
│   └── Aimlab.sln       # 解决方案文件
├── SFML/                 # SFML 3.1.0 库（头文件 + 导入库 + DLL）
├── assets/               # 游戏资源（背景图、字体）
└── .gitignore
```

## 设计说明

- 全部游戏逻辑采用 **header-only** 类实现
- OOP 设计模式：继承（GameMode → GridModeBase → SimpleMode/HardMode/RecoilMode）、组合（RecoilComponent）、对象池（TargetPool）、策略模式
- UI 采用分辨率比例缩放，全部尺寸用 `窗口尺寸 × 百分比` 计算
- 游戏状态机：MainMenu → Playing → Paused/Result → MainMenu
- 1:1 像素映射视图系统
