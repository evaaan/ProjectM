#pragma once

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
    void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo, ComponentHandle<ServerSingleton> server);
    void UpdateAllClients();
    void PollConnectionStateChanges();

};
