#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerInputSystem.h"

ServerInputSystem::ServerInputSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
}

void ServerInputSystem::init() {}

/* System behaviors */
void ServerInputSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }
}

void ServerInputSystem::updateServer()
{

}

/* System rendering */
void ServerInputSystem::render() {}
