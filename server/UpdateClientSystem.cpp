#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
#include <windows.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

#include "fbs/entity_generated.h"
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "UpdateClientSystem.h"
#include "Utilities.h"


UpdateClientSystem::UpdateClientSystem()
{
    // Add ComponentTypes the System acts on
    signature.addComponent<ServerSocketSingleton>();
}


void UpdateClientSystem::init()
{

    /* Only one server per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSocketSingleton> server_component;
        parentWorld->unpack(entity, server_component);

        /* Store a reference directly to the server singleton component. */
        server = server_component;
    }
}


/* Poll new connections and broadcast units to users */
void UpdateClientSystem::update(double dt)
{
    UpdateAllClients();
}


void UpdateClientSystem::UpdateAllClients()
{
    for (auto const& [hConn, uuid] : server->m_uuidMap)
    {
        // server->m_pInterface->SendMessageToConnection(hConn, msg, msg_len, k_nSteamNetworkingSend_Reliable, nullptr);
    }
}


/* System rendering */
void UpdateClientSystem::render() {}


