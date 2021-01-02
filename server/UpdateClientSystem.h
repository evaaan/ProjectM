#pragma once

#define LISTEN_PORT 35656

// Forward declarations
class System;
struct ServerSingleton;

class UpdateClientSystem : public System {
public:
    UpdateClientSystem();
    void init();
    void update(double dt);
    void render();

private:
    static void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void UpdateAllClients();
    void PollConnectionStateChanges();

    static UpdateClientSystem* s_pCallbackInstance;
    ComponentHandle<ServerSingleton> server;

};
