#include "Entity.h"
#include "System.h"
#include "World.h"
#include "CombatSystem.h"

CombatSystem::CombatSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
}

void CombatSystem::init() {}

/* System behaviors */
void CombatSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void CombatSystem::render() {}
