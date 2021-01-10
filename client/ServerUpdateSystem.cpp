#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerUpdateSystem.h"

ServerUpdateSystem::ServerUpdateSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientSocketSingleton>();
}

void ServerUpdateSystem::init() {}

/* System behaviors */
void ServerUpdateSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void ServerUpdateSystem::render() {}
