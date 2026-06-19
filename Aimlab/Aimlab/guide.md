# 2D FPS Aimlab Clone - OOP Architecture Guide

**Language**: C++20  
**Graphics Library**: SFML 3.1  
**Core OOP ideas**: inheritance, polymorphism, composition, object pool, strategy-style game modes.

---

## 1. Infrastructure

### `MathUtils`

Static utility class for shared math operations:

- `distance`: calculates distance between two points.
- `checkCirclePointCollision`: checks whether a point is inside a circular target.
- `getRandomFloat`: generates random floating-point values.

### `ResourceManager`

Singleton-style resource cache for textures, fonts, and sound buffers. It avoids loading the same resource repeatedly and centralizes resource access.

---

## 2. Weapon System

### `RecoilComponent`

Independent component responsible for weapon recoil:

- accumulates vertical recoil and horizontal shake after each shot;
- applies smoothing while firing;
- recovers toward zero when firing stops.

This is composition: `Weapon` owns a `RecoilComponent` instead of making recoil a separate inheritance hierarchy.

### `WeaponConfig`

Configuration object for concrete weapon differences:

- `ammoCapacity`
- `fireCooldown`
- `automatic`
- `recoil`

### `Weapon`

Base class for all weapons. It now owns the common shooting behavior:

- `fire()`: checks ammo, reload state, cooldown, then commits the shot.
- `update()`: updates cooldown, reload timer, and recoil.
- `reload()`: starts a reload when needed.
- `isAutomatic()`: returns whether holding the fire button should continue firing.

Concrete weapons such as `Usp`, `Ak47`, and `M4A1` only provide different `WeaponConfig` values. This removes duplicated `fire()` implementations while keeping polymorphic use through `Weapon* activeWeapon`.

---

## 3. Game Modes

### `GameMode`

Abstract interface for target spawning rules:

- `createSpawn(...)`
- `onTargetHit(...)`
- `reset()`
- `getDesiredActiveCount()`

`TargetSpawner` depends on this interface, not on specific modes.

### `GridModeBase`

Shared base class for grid-based modes. It handles:

- generating grid candidate positions;
- avoiding occupied positions;
- avoiding the last hit position;
- selecting a random spawn position;
- returning common `SpawnInfo`.

### Concrete Modes

- `SimpleMode`: 3x3 grid, large targets.
- `HardMode`: 5x5 grid, smaller targets.
- `RecoilMode`: 3x3 grid, targets require multiple hits.

These classes now mainly provide parameters, while repeated spawning logic lives in `GridModeBase`.

---

## 4. Target System

### `Target`

Abstract target base class. It defines common target state and behavior:

- position, radius, life timer;
- active/inactive state;
- hit detection;
- rendering;
- virtual `init`, `update`, and `onHit`.

### `StaticTarget`

Concrete target that stays in place until it is destroyed or expires. It also supports multi-hit targets by tracking `requiredHits` and `currentHits`.

### `TargetPool`

Object pool for targets:

- pre-allocates target objects;
- reuses inactive targets;
- stores them as `std::unique_ptr<Target>`.

This demonstrates RAII and avoids repeated allocation during gameplay.

### `TargetSpawner`

Coordinates spawning:

- asks the current `GameMode` for spawn data;
- acquires inactive targets from `TargetPool`;
- initializes targets with mode-specific parameters.

---

## 5. UI and Feedback

### `Crosshair`

Owns and draws the visual crosshair only. Recoil is applied to the camera view in `Game`, so `Crosshair` no longer needs to know about `Weapon`.

### `ScoreManager`

Tracks hit count, miss count, total shots, and accuracy. It keeps scoring logic separate from rendering and input.

---

## 6. Game Orchestration

### `Game`

Top-level coordinator:

- owns the SFML window;
- holds weapons and the active `Weapon*`;
- owns the target pool and spawner;
- owns the current `GameMode*`;
- handles input, update, hit detection, HUD text, and rendering.

`Game` is still the largest class. A future improvement would be splitting it into smaller classes such as `InputHandler`, `Hud`, `WeaponManager`, and `World`.

---

## 7. Current Architecture Strengths

- Weapons use shared base behavior plus configuration.
- Modes use polymorphism through `GameMode`.
- Repeated grid spawning logic is centralized in `GridModeBase`.
- Recoil is modeled as a reusable component.
- Targets are managed through an object pool with smart pointers.

## 8. Further Improvements

- Move large inline class implementations from `.h` files into `.cpp` files.
- Add more `Target` subclasses, such as `MovingTarget` or `ShrinkingTarget`.
- Make `ResourceManager` the only place where textures, fonts, and sound buffers are loaded.
- Split `Game` into smaller manager classes to reduce its responsibilities.
