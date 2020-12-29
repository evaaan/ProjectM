#pragma once
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>

#define LISTEN_PORT 35656

// Forward declarations
class System;

class UpdateClientSystem : public System {
public:
    UpdateClientSystem();
    void init();
    void update(double dt);
    void render();

private:
    static void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    HSteamListenSocket m_hListenSock;
    HSteamNetPollGroup m_hPollGroup;
    ISteamNetworkingSockets* m_pInterface;

};
