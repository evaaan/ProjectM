#pragma once
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#define LISTEN_PORT 35656

// Forward declarations
class System;

class UpdateClientSystem : public System {
public:
    UpdateClientSystem();
    void init();
    void update(double dt);
    void render();

    void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);

};
