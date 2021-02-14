#pragma once

// Forward declarations
class System;
struct WorldDeltaSingleton;
template <typename T>
struct ComponentHandle;
struct Dynamic;
struct Entity;

class PhysicsSystem : public System {
public:
    PhysicsSystem();
    void init();
    void update(double dt);
    void render();

private:

    ComponentHandle<WorldDeltaSingleton> worldDelta;
    ComponentHandle<KeyStateSingleton> keys;
    ComponentHandle<ServerSocketSingleton> server;

    void advanceTick(double dt);

    std::set<int> getSolidCollisions(Entity entity);
    void demoRunTick(double dt);

    bool checkCollision(ComponentHandle<Dynamic> one, ComponentHandle<Dynamic> two);
    void checkAllCollisions();
    void demoResolveSolidCollisions();
    void demoResolveLedgeCollisions();
    void updateTransforms();
};
