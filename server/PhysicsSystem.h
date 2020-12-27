#pragma once

// Forward declarations
class System;
template <typename T>
struct ComponentHandle;
class Dynamic;
class Entity;

class PhysicsSystem : public System {
public:
    PhysicsSystem();
    void init();
    void advanceTick(double dt);
    void update(double dt);
    bool checkCollision(ComponentHandle<Dynamic> one, ComponentHandle<Dynamic> two);
    void checkAllCollisions();
    std::set<int> getSolidCollisions(Entity entity);
    void demoRunTick(double dt);
    void demoResolveSolidCollisions();
    void demoResolveLedgeCollisions();
    void demoUpdateTransforms();
    void render();
};
