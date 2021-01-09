#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
#include <windows.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerInputSystem.h"
#include "Utilities.h"

/* Steam Sockets init and shutdown helpers */
SteamNetworkingMicroseconds g_logTimeZero;

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - g_logTimeZero;
    odslog("%10.6f %s\n", time * 1e-6, pszMsg);
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

ServerInputSystem* ServerInputSystem::s_pCallbackInstance = nullptr;  // static member definition must be in translation unit


ServerInputSystem::ServerInputSystem(const char *server_addr) : m_server_addr(server_addr)
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientSocketSingleton>();
}

/* Cleanup client connections */
ServerInputSystem::~ServerInputSystem()
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

void ServerInputSystem::init()
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
    odslog("Connecting to server at " << m_server_addr);
    SteamNetworkingConfigValue_t opt;
    client->opt->SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnStatusChangedCallback);
    client->m_hConnection = client->m_pInterface->ConnectByIPAddress(*client->m_addrServer, 1, &opt);
    if (client->m_hConnection == k_HSteamNetConnection_Invalid)
        throw std::runtime_error(std::string("Failed to connect to server!"));
}

/* System behaviors */
void ServerInputSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);
    }
}


void ServerInputSystem::ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    /* This static callback function is passed as type void* to SetPtr,
    but we need to call a member function to access server connection information. */
    s_pCallbackInstance->OnConnStatusChange(pInfo);
}


void ServerInputSystem::OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
}


/* System rendering */
void ServerInputSystem::render() {}
