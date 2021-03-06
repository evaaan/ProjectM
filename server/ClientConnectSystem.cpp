#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
#include <assert.h>
#include <windows.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "fbs/entity_generated.h"
#include "EntityHandle.h"
#include "System.h"
#include "World.h"
#include "ClientConnectSystem.h"
#include "Utilities.h"


#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


/* Steam Sockets init and shutdown helpers */
SteamNetworkingMicroseconds g_logTimeZero;

static void DebugOutput(ESteamNetworkingSocketsDebugOutputType eType, const char* pszMsg)
{
    SteamNetworkingMicroseconds time = SteamNetworkingUtils()->GetLocalTimestamp() - g_logTimeZero;
    odslog("[SteamNetworkingSockets Debug] " << pszMsg << "\n");
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

ClientConnectSystem* ClientConnectSystem::s_pCallbackInstance = nullptr;  // static member definition must be in translation unit

ClientConnectSystem::ClientConnectSystem()
{
    // Add ComponentTypes the System acts on
    signature.addComponent<ServerSocketSingleton>();

    // Client entity is created on connection
    signature.addComponent<WorldDeltaSingleton>();
}

/* Cleanup server connections */
ClientConnectSystem::~ClientConnectSystem()
{

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
    ShutdownSteamDatagramConnectionSockets();
}


void ClientConnectSystem::init()
{
    server = parentWorld->getSingletonComponent<ServerSocketSingleton>();
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();

    InitSteamDatagramConnectionSockets();


    // Initialize SteamNetworking interface
    server->m_pInterface = SteamNetworkingSockets();
    server->serverLocalAddr = std::make_shared<SteamNetworkingIPAddr>();
    server->opt = std::make_shared<SteamNetworkingConfigValue_t>();

    // Start listening
    server->serverLocalAddr->Clear();
    server->serverLocalAddr->m_port = server->listen_port;
    server->opt->SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)ConnStatusChangedCallback);
    server->m_hListenSock = server->m_pInterface->CreateListenSocketIP(*server->serverLocalAddr, 1, server->opt.get());
    if (server->m_hListenSock == k_HSteamListenSocket_Invalid)
        throw std::runtime_error(std::string("Unable to listen on port ") + std::to_string(server->listen_port));
    server->m_hPollGroup = server->m_pInterface->CreatePollGroup();
    if (server->m_hPollGroup == k_HSteamNetPollGroup_Invalid)
        throw std::runtime_error(std::string("Unable to poll on port ") + std::to_string(server->listen_port));

    odslog("Server started listening on port " << server->listen_port << "\n");
}

/* System behaviors */
void ClientConnectSystem::update(double dt)
{
    PollConnectionStateChanges();
}


/* Callback function for CreateListenSocketIP */
void ClientConnectSystem::ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    /* This static callback function is passed as type void* to SetPtr (why would they do that???),
    but we need to call a member function to access server connection information. */
    s_pCallbackInstance->OnConnStatusChange(pInfo);
}

void ClientConnectSystem::PollConnectionStateChanges()
{
    s_pCallbackInstance = this;
    server->m_pInterface->RunCallbacks();

}

/// copied from isteamnetworkingsockets.h :
/// This callback is posted whenever a connection is created, destroyed, or changes state.
/// The m_info field will contain a complete description of the connection at the time the
/// change occurred and the callback was posted.  In particular, m_eState will have the
/// new connection state.
///
/// You will usually need to listen for this callback to know when:
/// - A new connection arrives on a listen socket.
///   m_info.m_hListenSocket will be set, m_eOldState = k_ESteamNetworkingConnectionState_None,
///   and m_info.m_eState = k_ESteamNetworkingConnectionState_Connecting.
///   See ISteamNetworkigSockets::AcceptConnection.
/// - A connection you initiated has been accepted by the remote host.
///   m_eOldState = k_ESteamNetworkingConnectionState_Connecting, and
///   m_info.m_eState = k_ESteamNetworkingConnectionState_Connected.
///   Some connections might transition to k_ESteamNetworkingConnectionState_FindingRoute first.
/// - A connection has been actively rejected or closed by the remote host.
///   m_eOldState = k_ESteamNetworkingConnectionState_Connecting or k_ESteamNetworkingConnectionState_Connected,
///   and m_info.m_eState = k_ESteamNetworkingConnectionState_ClosedByPeer.  m_info.m_eEndReason
///   and m_info.m_szEndDebug will have for more details.
///   NOTE: upon receiving this callback, you must still destroy the connection using
///   ISteamNetworkingSockets::CloseConnection to free up local resources.  (The details
///   passed to the function are not used in this case, since the connection is already closed.)
/// - A problem was detected with the connection, and it has b+een closed by the local host.
///   The most common failure is timeout, but other configuration or authentication failures
///   can cause this.  m_eOldState = k_ESteamNetworkingConnectionState_Connecting or
///   k_ESteamNetworkingConnectionState_Connected, and m_info.m_eState = k_ESteamNetworkingConnectionState_ProblemDetectedLocally.
///   m_info.m_eEndReason and m_info.m_szEndDebug will have for more details.
///   NOTE: upon receiving this callback, you must still destroy the connection using
///   ISteamNetworkingSockets::CloseConnection to free up local resources.  (The details
///   passed to the function are not used in this case, since the connection is already closed.)
///
/// Remember that callbacks are posted to a queue, and networking connections can
/// change at any time.  It is possible that the connection has already changed
/// state by the time you process this callback.
///
/// Also note that callbacks will be posted when connections are created and destroyed by your own API calls.
void ClientConnectSystem::OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo)
{
    // What's the state of the connection?
    switch (pInfo->m_info.m_eState)
    {
    case k_ESteamNetworkingConnectionState_None:
        break;

    case k_ESteamNetworkingConnectionState_ClosedByPeer:

        // A problem was detected with the connection, and has been closed by the local host. Most common failure is timeout.
    case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    {
        // Ignore if they are no longer connected.
        if (pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connected)
        {
            // Locate the client.
            auto itClient = server->m_uuidMap.find(pInfo->m_hConn);
            assert(itClient != server->m_uuidMap.end());

            const char* problemDescription;
            if (pInfo->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally)
            {
                problemDescription = "problem detected locally";
            }
            else
            {
                problemDescription = "closed by peer";
            }
            odslog("Connection " << pInfo->m_info.m_szConnectionDescription << problemDescription << ", reason " << pInfo->m_info.m_eEndReason << ": " << pInfo->m_info.m_szEndDebug << "\n");
            server->m_uuidMap.erase(itClient);
        }
        else
        {
            assert(pInfo->m_eOldState == k_ESteamNetworkingConnectionState_Connecting);
        }
        // Clean up the connection.
        server->m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
        break;

    }

    case k_ESteamNetworkingConnectionState_Connecting:
    {
        // Make sure it's a new connection
        assert(server->m_uuidMap.find(pInfo->m_hConn) == server->m_uuidMap.end());

        odslog("Connection request from: " << pInfo->m_info.m_szConnectionDescription << "\n");

        // Try to accept the connection
        if (server->m_pInterface->AcceptConnection(pInfo->m_hConn) != k_EResultOK)
        {
            // If it fails, clean-up the connection
            server->m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            odslog("Can't accept the connection. (It was already closed?)\n");
            break;
        }

        // Assign the poll group!
        if (!server->m_pInterface->SetConnectionPollGroup(pInfo->m_hConn, server->m_hPollGroup))
        {
            // If it fails, clean-up the connection
            server->m_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
            odslog("Failed to set the poll group?\n");
            break;
        }

        // Assign a new UUID (it was on this line I gave into boost)
        auto uuid = boost::uuids::random_generator()();
        server->m_uuidMap[pInfo->m_hConn] = uuid;
        server->m_hConnMap[uuid] = pInfo->m_hConn;


        // Give em a nickname.
        char nick[64];
        sprintf_s(nick, "Peasant%d", 10000 + (rand() % 100000));
        server->m_nickMap[uuid] = nick;

        // Set the connection name (not required)
        server->m_pInterface->SetConnectionName(pInfo->m_hConn, nick);
        odslog("New user connected: " << nick << "\n");

        // Create an entity for the player
        auto id = addClientEntity(nick);
        server->m_idMap[uuid] = id;

        // Initialize world for the player; notify all clients of all other players out there
        updateAllClients();
        break;
    }

    case k_ESteamNetworkingConnectionState_Connected:
        // We will get a callback immediately after accepting the connection. 
        break;

    default:
        break;
    }
}

/* Notify all clients of all existing players */
void ClientConnectSystem::updateAllClients()
{
    // for (auto const& [uuid, entity_id] : server->m_idMap)
    for (auto const& entity_id: parentWorld->getEntityIds())
    {
        worldDelta->state[entity_id].setDefault();
    }
}

void ClientConnectSystem::destroyClient(boost::uuids::uuid uuid)
{
    int entity_id = server->m_idMap[uuid];

    // Delete ECS references
    parentWorld->destroyEntity({ entity_id });

    // Delete netcode references
    server->m_hConnMap.erase(uuid);
    server->m_nickMap.erase(uuid);
    server->m_idMap.erase(uuid);
}


int ClientConnectSystem::addClientEntity(const char* nick)
{
    // Create Entity and add Components
    EntityHandle client_entity = parentWorld->createEntity();
    client_entity.addComponent(AnimationStore(), Combat(), Transform(), Dynamic(), Outline(), Player());

    int id = client_entity.entity.uuid;
    int x_pos = 200 + (rand() % 800);
    int y_pos = 200 + (rand() % 100);

    // Build Transform and Dynamic Components
    auto transform = client_entity.getComponent<Transform>();
    transform->width = 64;
    transform->height = 64;
    transform->x = x_pos;
    transform->y = y_pos;

    auto dynamic = client_entity.getComponent<Dynamic>();
    dynamic->width = transform->width;
    dynamic->height = transform->height;
    dynamic->pos.x = x_pos;
    dynamic->pos.y = y_pos;
    dynamic->vel.x = 0.0;
    dynamic->vel.y = 0.0;
    dynamic->accel.x = 0.0;
    dynamic->accel.y = 3000.0;
    dynamic->type = BodyType::Mob;
    odsloga("Added client entity, id: " << id << "\n");

    auto combat = client_entity.getComponent<Combat>();
    combat->owner = id;

    // Encode animation data
    auto animation = client_entity.getComponent<AnimationStore>();
    animation->store = { "idle", "weapon_idle"};

    // Set player nickname and entity ID
    auto player = client_entity.getComponent<Player>();
    player->id = id;
    player->username = nick;
    player->attacking = false;  // in an attack animation
    player->in_combat = false;  // currently taking damage
    player->start_combat = false;  // started combat this tick

    // Set mask so that ClientUpdateSystem updates the client(s)
    worldDelta->state[id] = ComponentMask();
    worldDelta->state[id].addDefaultComponent<AnimationStore, Transform, Dynamic, Outline, Player>();
    worldDelta->state[id].setDefault();

    return id;
}

void ClientConnectSystem::SendStringToClient(HSteamNetConnection conn, const char* str)
{
    server->m_pInterface->SendMessageToConnection(conn, str, (uint32)strlen(str), k_nSteamNetworkingSend_Reliable, nullptr);
}

/* System rendering */
void ClientConnectSystem::render() {}
