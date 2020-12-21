#pragma once
#include <bitset>
#include <memory>
#include <vector>

#include "ComponentManager.h"
#include "Entity.h"
#include "ComponentMask.h"

/* System template

class PhysicsSystem : public System {
public:
    void init();
    void update(int dt);
    void render();
};

void PhysicsSystem::init() {}
void PhysicsSystem::render() {}
void PhysicsSystem::update(double dt) {}
*/

// Forward declarations
class World;

class System {
public:
    System() = default;
    virtual ~System() = default;
    System(const System&) = default;
    System& operator=(const System&) = default;
    System(System&&) = default;
    System& operator=(System&&) = default;

    /*
     * Called before the game starts but after the world initializes
     */
    virtual void init() {};

    /*
     * Called every game update 
     */
    virtual void update(double dt) {};

    /*
     * Called every frame
     */
    virtual void render() {};

    /*
     * When a system is added to the world, the world will register itself
     */
    void registerWorld(std::shared_ptr<World> world);

    /*
     * When a component is added such that this system should begin acting on it,
     * register will be called.
     */
    void registerEntity(Entity const& entity);

    /*
     * If a component is removed from an entity such that the system should stop
     * acting on it, unRegister will be called.
     */
    void unRegisterEntity(Entity const& entity);
    ComponentMask getSignature();

protected:
    std::vector<Entity> registeredEntities;
    std::shared_ptr<World> parentWorld;
    ComponentMask signature;
};
