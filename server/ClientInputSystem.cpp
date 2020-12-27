#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ClientInputSystem.h"
#include "Component.h"

ClientInputSystem::ClientInputSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientConnection>();
    signature.addComponent<ClientInput>();
    signature.addComponent<PollGroup>();
}

void ClientInputSystem::init() {}

/* System behaviors */
void ClientInputSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void ClientInputSystem::render() {}
