# 2D 瞄准训练游戏 (Aimlab Clone) - C++ 面向对象架构大纲

**开发语言**: C++ (C++11/14/17)
**图形库**: SFML (Simple and Fast Multimedia Library)
**核心编程思想**: 面向对象编程 (OOP)、组合 (Composition)、多态 (Polymorphism)、对象池 (Object Pool)

---

## 1. 基础支撑层 (Infrastructure)

提供全局通用的工具和资源管理，避免内存泄漏和重复加载。

### `ResourceManager` (资源管理类 - 单例模式)
* **功能模块**: 负责统一加载和缓存图片、音效、字体，确保同一种资源在内存中只有一份。
* **核心成员变量**:
    * `std::unordered_map<std::string, sf::Texture> textures`
    * `std::unordered_map<std::string, sf::SoundBuffer> soundBuffers`
    * `std::unordered_map<std::string, sf::Font> fonts`
* **核心方法**:
    * `static ResourceManager& getInstance()`: 获取全局唯一的单例实例。
    * `void loadTexture(const std::string& name, const std::string& filename)`: 从路径加载贴图并存入 Map。
    * `sf::Texture& getTexture(const std::string& name)`: 根据名字获取贴图引用。

### `MathUtils` (数学工具类 - 静态类)
* **功能模块**: 处理游戏中所有的数学计算，特别是碰撞检测和随机数生成。
* **核心方法**:
    * `static float distance(sf::Vector2f p1, sf::Vector2f p2)`: 计算两点之间的欧几里得距离。
    * `static bool checkCirclePointCollision(sf::Vector2f center, float radius, sf::Vector2f point)`: 圆与点的碰撞检测（用于鼠标点击判定）。
    * `static float getRandomFloat(float min, float max)`: 生成指定范围的随机浮点数。

---

## 2. 核心武器系统 (Weapon System)

利用“组合”与“多态”设计不同手感的枪械，后坐力采用线性累加与匀速回正模型。

### `RecoilComponent` (简单后坐力组件)
* **功能模块**: 管理开火时的屏幕抖动（上抬和左右震荡）以及匀速回正。
* **核心成员变量**:
    * `float currentOffsetY`: 当前竖直向上的偏移量。
    * `float currentOffsetX`: 当前水平方向的偏移量。
    * `float kickY`: 每次开火固定的竖直上抬数值。
    * `float shakeX`: 每次开火水平随机震荡的最大范围。
    * `float recoverySpeed`: 每秒匀速回正的像素值。
* **核心方法**:
    * `void applyRecoil()`: 增加上抬和随机水平偏移 (`currentOffsetY += kickY; currentOffsetX += random(-shakeX, shakeX)`).
    * `void update(float deltaTime)`: 按照 `recoverySpeed * deltaTime` 的速度让当前的 X 和 Y 偏移量匀速趋近于 0。
    * `sf::Vector2f getOffset() const`: 返回 `(currentOffsetX, -currentOffsetY)` 供准星渲染。

### `Weapon` (武器抽象基类)
* **功能模块**: 定义所有枪械的通用接口与基础属性。
* **核心成员变量**:
    * `RecoilComponent recoil`: 组合后坐力组件。
    * `int ammoCapacity`: 弹匣最大容量。
    * `int currentAmmo`: 当前子弹数。
    * `float fireCooldown`: 射击间隔（控制射速）。
    * `float timeSinceLastFire`: 射击 CD 计时器。
    * `sf::Sound fireSound`: 枪声音效对象。
* **核心方法**:
    * `virtual bool fire() = 0`: 纯虚函数。检查弹药和 CD，触发后坐力并返回 true/false。
    * `virtual void update(float deltaTime)`: 更新 CD 计时器，调用 `recoil.update(deltaTime)`。
    * `void reload()`: 重新装填子弹。

### 派生类: `Pistol`, `Rifle`, `Sniper`
* **`Pistol`**: 半自动手枪。在 `fire()` 中需强制要求松开鼠标才能再次射击。后坐力上抬中等，回正极快。
* **`Rifle`**: 全自动步枪。按住左键即可连续调用 `fire()`。单发后坐力小，但回正慢，连发会导致准星持续上移。
* **`Sniper`**: 狙击枪。射击间隔长（`fireCooldown` 极大），开火瞬间后坐力极大。

---

## 3. 实体与内存管理系统 (Entity & Memory)

使用对象池（Object Pool）技术优化高频的靶子生成与销毁操作。

### `Target` (标靶抽象基类)
* **功能模块**: 定义靶子的基本属性。
* **核心成员变量**:
    * `sf::CircleShape shape`: SFML 渲染形状。
    * `sf::Vector2f position`: 坐标。
    * `float radius`: 半径大小。
    * `float lifeTimer`: 存活倒计时。
    * `bool isActive`: 对象池状态标记（激活/休眠）。
* **核心方法**:
    * `virtual void init(sf::Vector2f startPos, float r, float lifeTime) = 0`: 初始化/重置靶子状态。
    * `virtual void update(float deltaTime) = 0`: 扣减倒计时，超时则自我隐藏。
    * `void render(sf::RenderWindow& window)`: 绘制自身。
    * `bool isHit(sf::Vector2f mousePos)`: 内部调用 `MathUtils` 判断是否被击中。

### 派生类: `StaticTarget` (静态靶)
* 继承自 `Target`，在原地停留直到被击中或超时。

### `TargetPool` (标靶对象池)
* **功能模块**: 预先分配内存，避免游戏进行中频繁使用 `new` 和 `delete`。
* **核心成员变量**:
    * `std::vector<std::unique_ptr<Target>> pool`: 预先分配好的对象集合（例如 50 个）。
* **核心方法**:
    * `Target* acquireTarget()`: 寻找并返回一个 `isActive = false` 的空闲靶子指针。
    * `void releaseTarget(Target* target)`: 将靶子的 `isActive` 设为 false，隐藏它以供下次复用。

### `TargetSpawner` (靶子生成器)
* **功能模块**: 控制游戏节奏，决定何时、何地生成靶子。
* **核心成员变量**:
    * `TargetPool* poolRef`: 内存池的指针/引用。
    * `float spawnInterval`: 生成间隔。
    * `float timeSinceLastSpawn`: 生成计时器。
* **核心方法**:
    * `void update(float deltaTime)`: 计时，超时则调用 `poolRef->acquireTarget()`，并在屏幕随机合法坐标处初始化它。

---

## 4. 玩家交互与界面反馈 (Player & HUD)

### `Crosshair` (准星类)
* **功能模块**: 绘制鼠标位置，并实时叠加武器后坐力。
* **核心成员变量**:
    * `sf::Sprite sprite`: 准星贴图。
    * `Weapon* currentWeapon`: 当前持有的武器引用。
* **核心方法**:
    * `void updatePosition(sf::Vector2i mousePhysicalPos)`: 实际渲染坐标 = `mousePhysicalPos` + `currentWeapon->recoil.getOffset()`。
    * `void render(sf::RenderWindow& window)`: 隐藏系统默认鼠标，绘制准星贴图。

### `ScoreManager` (计分统计类)
* **功能模块**: 记录并计算玩家的游戏表现。
* **核心成员变量**:
    * `int hits`, `int misses`, `int totalShots`.
* **核心方法**:
    * `void recordHit()`: 命中数 +1。
    * `void recordMiss()`: Miss 数 +1。
    * `float getAccuracy() const`: 计算命中率 (`hits / totalShots * 100.0f`)。

---

## 5. 顶层主控逻辑 (The Engine)

### `Game` (游戏主控类)
* **功能模块**: 封装 SFML 窗口，驱动所有子系统，管理游戏主循环。
* **核心成员变量**:
    * `sf::RenderWindow window`: 主渲染窗口。
    * `Weapon* activeWeapon`: 当前使用的武器（多态指针）。
    * `TargetSpawner spawner`: 靶子生成器。
    * `TargetPool targetPool`: 靶子对象池。
    * `Crosshair crosshair`: 准星系统。
    * `ScoreManager scoreManager`: 计分系统。
* **核心方法**:
    * `void run()`: 包含标准游戏主循环。
    * `void processEvents()`: 处理关闭窗口、鼠标移动、按键切换武器等事件。
        * **开火逻辑**: 若检测到鼠标左键点击，首先调用 `activeWeapon->fire()`。如果返回 true（开火成功），记录一次射击，获取准星当前实际位置（包含后坐力），然后遍历活着的靶子调用 `isHit()`。
    * `void update(sf::Time deltaTime)`: 依次调用武器、生成器、存活靶子的 `update()`。
    * `void render()`: 执行 `window.clear()` -> 渲染背景 -> 渲染所有激活靶子 -> 渲染 UI 分数 -> 渲染准星 -> `window.display()`。