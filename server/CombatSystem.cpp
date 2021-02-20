#include "Entity.h"
#include "KeyMap.h"
#include "System.h"
#include "World.h"
#include "CombatSystem.h"
#include "Utilities.h"

/* The combat system reads attack actions from the player,
collision data from the PhysicsSystem, and updates the Player
Component information accordingly. */
CombatSystem::CombatSystem()
{
    // Add ComponentTypes the System acts on
    signature.addComponent<Combat, Player>();
}

void CombatSystem::init()
{

    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
    keys = parentWorld->getSingletonComponent<KeyStateSingleton>();
    collisions = parentWorld->getSingletonComponent<CollisionSingleton>();
}

/* System behaviors */
void CombatSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<Combat> combat;
        parentWorld->unpack(entity, combat);
    }

}

/* System rendering */
void CombatSystem::render() {}
