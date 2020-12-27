#include "Entity.h"
#include "System.h"
#include "World.h"
#include "PlayerSystem.h"

PlayerSystem::PlayerSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
}

void PlayerSystem::init() {}

/* System behaviors */
void PlayerSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void PlayerSystem::render() {}
