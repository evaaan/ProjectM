#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "fbs/entity_generated.h"
#include "Entity.h"
#include "System.h"
#include "UpdateClientSystem.h"
#include "World.h"
#include "Utilities.h"


UpdateClientSystem::UpdateClientSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ServerSingleton>();
    // signature.addComponent<PlayerDelta>();
    // signature.addComponent<WorldDeltaSingleton>();
}

/* Cleanup server connections */
UpdateClientSystem::~UpdateClientSystem()
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSingleton> server;
        parentWorld->unpack(entity, server);


        if (server->m_pInterface)
        {
            if (server->m_hListenSock)
            {
                server->m_pInterface->CloseListenSocket(server->m_hListenSock);
                server->m_hListenSock = k_HSteamListenSocket_Invalid;
            }
            if (server->m_hPollGroup)
            {
                server->m_pInterface->DestroyPollGroup(server->m_hPollGroup);
                server->m_hPollGroup = k_HSteamNetPollGroup_Invalid;
            }
        }
    }
}

void UpdateClientSystem::init()
{
    /* Only one server per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSingleton> server_component;
        parentWorld->unpack(entity, server_component);

        /* Store a reference directly to the server component. */
        server = server_component;
    }


    // Initialize SteamNetworking interface
    server->m_pInterface = SteamNetworkingSockets();
    server->serverLocalAddr = std::make_unique<SteamNetworkingIPAddr>();
    server->opt = std::make_unique<SteamNetworkingConfigValue_t>();

    // Start listening
    server->serverLocalAddr->Clear();
    server->serverLocalAddr->m_port = server->listen_port;
    server->opt->SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void *)ConnStatusChangedCallback); // void *
    server->m_hListenSock = server->m_pInterface->CreateListenSocketIP(*server->serverLocalAddr, 1, server->opt.get());
    if (server->m_hListenSock == k_HSteamListenSocket_Invalid)
        throw std::runtime_error(std::string("Unable to listen on port ") + std::to_string(server->listen_port));
    server->m_hPollGroup = server->m_pInterface->CreatePollGroup();
    if (server->m_hPollGroup == k_HSteamNetPollGroup_Invalid)
        throw std::runtime_error(std::string("Unable to poll on port ") + std::to_string(server->listen_port));

    odslog("Server started listening on port: " << server->listen_port);
}


/* Poll new connections and broadcast units to users */
void UpdateClientSystem::update(double dt)
{
    UpdateAllClients();
    PollConnectionStateChanges();
}

/* Callback function for CreateListenSocketIP */
void UpdateClientSystem::ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    /* This static callback function is passed as type void* to SetPtr (why would they do that???),
    but we need to call a member function to access server connection information. */
    s_pCallbackInstance->OnConnStatusChange(pInfo);
}

void UpdateClientSystem::PollConnectionStateChanges()
{
    s_pCallbackInstance = this;
    server->m_pInterface->RunCallbacks();

}

void UpdateClientSystem::OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    char temp[512];

    // What's the state of the connection?
    switch (pInfo->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_None:
        break;
    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    {

    }
    case k_ESteamNetworkingConnectionState_Connecting:
    {
        // Make sure it's a new connection
        assert(server->m_clientMap.find(pInfo->m_hConn) == server->m_clientMap.end());
        odslog("Connection request from: " << pInfo->m_info.m_szConnectionDescription);
    }
    case k_ESteamNetworkingConnectionState_Connected:
        // We will get a callback immediately after accepting the connection. 
        break;
    default:
        break;
    }
}



void UpdateClientSystem::UpdateAllClients()
{
}

/* System rendering */
void UpdateClientSystem::render() {}