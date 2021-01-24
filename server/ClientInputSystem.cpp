#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ClientInputSystem.h"
#include "Component.h"

/* Receive input messages from Client */
ClientInputSystem::ClientInputSystem()
{
    signature.addComponent<ClientSocketSingleton>();
    signature.addComponent<KeyState>();
}

void ClientInputSystem::init() {}

/* System behaviors */
void ClientInputSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<KeyState> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void ClientInputSystem::render() {}
