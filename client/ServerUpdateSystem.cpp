#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerUpdateSystem.h"
#include "Utilities.h"

ServerUpdateSystem::ServerUpdateSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientSocketSingleton>();
}

void ServerUpdateSystem::init()
{
    /* Only one client socket component exists */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ClientSocketSingleton> client_component;
        parentWorld->unpack(entity, client_component);

        /* Store a reference directly to the client singleton component. */
        client = client_component;
    }
}

/* System behaviors */
void ServerUpdateSystem::update(double dt)
{
    // Read incoming messages into an ISteamNetworkingMessage array
    ISteamNetworkingMessage* pIncomingMsg[8];
    int numMsgs = client->m_pInterface->ReceiveMessagesOnConnection(client->m_hConnection, pIncomingMsg, 8);
    if (numMsgs < 0)
        throw std::runtime_error("Invalid connection handle!");
    for (int num = 0; num < numMsgs; num++)
    {
        if (pIncomingMsg != nullptr && pIncomingMsg[num] != nullptr)
        {
            std::string nick((char*)pIncomingMsg[num]->m_pData, pIncomingMsg[num]->m_cbSize);
            odsloga("Received message: " << nick << "\n");

            pIncomingMsg[num]->Release();
        }
    }
}

/* System rendering */
void ServerUpdateSystem::render() {}
