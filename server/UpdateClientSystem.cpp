#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "fbs/entity_generated.h"
#include "Entity.h"
#include "System.h"
#include "UpdateClientSystem.h"
#include "World.h"

UpdateClientSystem::UpdateClientSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientConnection>();
    signature.addComponent<PlayerDelta>();
    signature.addComponent<WorldDeltaSingleton>();
}

void UpdateClientSystem::init()
{
    // Select interface
    auto m_pInterface = SteamNetworkingSockets();

    // Start listening
    SteamNetworkingIPAddr serverLocalAddr;
    serverLocalAddr.Clear();
    serverLocalAddr.m_port = LISTEN_PORT;
    SteamNetworkingConfigValue_t opt;
    opt.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnStatusChangedCallback);
    m_hListenSock = m_pInterface->CreateListenSocketIP(serverLocalAddr, 1, &opt);
    if (m_hListenSock == k_HSteamListenSocket_Invalid)
        throw std::runtime_error(std::string("Unable to listen on port ") + std::to_string(LISTEN_PORT));
    m_hPollGroup = m_pInterface->CreatePollGroup();
    if (m_hPollGroup == k_HSteamNetPollGroup_Invalid)
        throw std::runtime_error(std::string("Unable to poll on port ") + std::to_string(LISTEN_PORT));

    OutputDebugStringA("Server has started listening.");

}

void UpdateClientSystem::ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{

}

/* System behaviors */
void UpdateClientSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void UpdateClientSystem::render() {}