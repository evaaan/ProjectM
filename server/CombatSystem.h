#pragma once

// Forward declarations
class System;
struct WorldDeltaSingleton;
template <typename T>
struct ComponentHandle;


class CombatSystem : public System {
public:
    CombatSystem();
    void init();
    void update(double dt);
    void render();

private:
    ComponentHandle<WorldDeltaSingleton> worldDelta;
    ComponentHandle<KeyStateSingleton> keys;
    ComponentHandle<CollisionSingleton> collisions;
};
