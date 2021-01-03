#pragma once

// Forward declarations
class System;
struct ServerSingleton;
template <typename T>
struct ComponentHandle;

class UpdateClientSystem : public System {
public:
    UpdateClientSystem();
    ~UpdateClientSystem();
    void init();
    void update(double dt);
    void render();

    static UpdateClientSystem* s_pCallbackInstance;  // Pointer to 'this' for CreateListenSocketIP callback

private:
    static void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void UpdateAllClients();
    void PollConnectionStateChanges();

    ComponentHandle<ServerSingleton> server;

};
