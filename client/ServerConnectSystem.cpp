#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
#include <windows.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerConnectSystem.h"
#include "Utilities.h"

/* Steam Sockets init and shutdown helpers */
SteamNetworkingMicroseconds g_logTimeZero;

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - g_logTimeZero;
    odslog(time * 1e-6 << pszMsg << "\n");
    if (eType == k_ESteamNetworkingSocketsDebugOutputType_Bug)
    {
        fflush(stdout);
        fflush(stderr);
        ExitProcess(1);
    }
}


static void InitSteamDatagramConnectionSockets()
{
#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
    SteamDatagramErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg))
        odslogfatal("GameNetworkingSockets_Init failed.  %s", errMsg);
#else
    SteamDatagramClient_SetAppID(570); // Just set something, doesn't matter what
    //SteamDatagramClient_SetUniverse( k_EUniverseDev );

    SteamDatagramErrMsg errMsg;
    if (!SteamDatagramClient_Init(true, errMsg))
        odslogfatal("SteamDatagramClient_Init failed.  %s", errMsg);

    // Disable authentication when running with Steam, for this
    // example, since we're not a real app.
    //
    // Authentication is disabled automatically in the open-source
    // version since we don't have a trusted third party to issue
    // certs.
    SteamNetworkingUtils()->SetGlobalConfigValueInt32(k_ESteamNetworkingConfig_IP_AllowWithoutAuth, 1);
#endif

    g_logTimeZero = SteamNetworkingUtils()->GetLocalTimestamp();

    SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, DebugOutput);
}


static void ShutdownSteamDatagramConnectionSockets()
{
    // Give connections time to finish up.  This is an application layer protocol
    // here, it's not TCP.  Note that if you have an application and you need to be
    // more sure about cleanup, you won't be able to do this.  You will need to send
    // a message and then either wait for the peer to close the connection, or
    // you can pool the connection to see if any reliable data is pending.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef STEAMNETWORKINGSOCKETS_OPENSOURCE
    GameNetworkingSockets_Kill();
#else
    SteamDatagramClient_Kill();
#endif
}

ServerConnectSystem* ServerConnectSystem::s_pCallbackInstance = nullptr;  // static member definition must be in translation unit


ServerConnectSystem::ServerConnectSystem(const char *server_addr) : m_server_addr(server_addr)
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientSocketSingleton>();
}

/* Cleanup client connections */
ServerConnectSystem::~ServerConnectSystem()
{
    if (client->m_pInterface)
    {
        if (client->m_hListenSock)
        {
            client->m_pInterface->CloseListenSocket(client->m_hListenSock);
            client->m_hListenSock = k_HSteamListenSocket_Invalid;
        }
    }
    ShutdownSteamDatagramConnectionSockets();
}

void ServerConnectSystem::init()
{

    /* Only one client socket component exists */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ClientSocketSingleton> client_component;
        parentWorld->unpack(entity, client_component);

        /* Store a reference directly to the client singleton component. */
        client = client_component;
    }

    InitSteamDatagramConnectionSockets();

    // Initialize SteamNetworking Interface
    client->m_pInterface = SteamNetworkingSockets();

    client->m_addrServer = std::make_shared<SteamNetworkingIPAddr>();
    if (!client->m_addrServer->ParseString(m_server_addr))
        throw std::runtime_error(std::string("Invalid server address: ") + std::string(m_server_addr));

    client->opt = std::make_shared<SteamNetworkingConfigValue_t>();
    odslog("Attempting to connect to " << m_server_addr << "\n");
    client->opt->SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnStatusChangedCallback);
    client->m_hConnection = client->m_pInterface->ConnectByIPAddress(*client->m_addrServer, 1, client->opt.get());
    if (client->m_hConnection == k_HSteamNetConnection_Invalid)
        throw std::runtime_error(std::string("Failed to connect to server!"));
}

/* System behaviors */
void ServerConnectSystem::update(double dt)
{
    PollConnectionStateChanges();
}


void ServerConnectSystem::ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    /* This static callback function is passed as type void* to SetPtr,
    but we need to call a member function to access server connection information. */
    s_pCallbackInstance->OnConnStatusChange(pInfo);
}


void ServerConnectSystem::OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    if (pInfo->m_hConn != client->m_hConnection && client->m_hConnection != k_HSteamNetConnection_Invalid)
        throw std::runtime_error(std::string("Invalid HSteamNetConnection!"));

    switch (pInfo->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_None:
        // ignore the callback generated when we destroy connections
        break;

    case k_ESteamNetworkingConnectionState_ClosedByPeer:
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    {
        if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting)
        {
            odslog("Connection failed: " << pInfo->m_info.m_szEndDebug << "\n");
        }
        else if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
        {
            odslog("Problem detected locally, lost contact to remote host: " << pInfo->m_info.m_szEndDebug << "\n");
        }
        else
            odslog("Server ended the connection.\n");

        // Clean up the connection
        client->m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
        client->m_hConnection = k_HSteamNetConnection_Invalid;
        break;
    }
    case k_ESteamNetworkingConnectionState_Connecting:
    {
        odslog("Connecting to server...\n");
        break;
    }
    case k_ESteamNetworkingConnectionState_Connected:
    {
        odslog("Connected to server!\n");
        break;
    }
    default:
        break;
    }
}

/* Poll for connection status changes */
void ServerConnectSystem::PollConnectionStateChanges()
{
    s_pCallbackInstance = this;
    client->m_pInterface->RunCallbacks();
}


/* System rendering */
void ServerConnectSystem::render() {}
