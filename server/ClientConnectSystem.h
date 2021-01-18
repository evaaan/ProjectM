#pragma once

// Forward declarations
class System;
struct ServerSocketSingleton;
template <typename T>
struct ComponentHandle;

class ClientConnectSystem : public System {
public:
    ClientConnectSystem();
    ~ClientConnectSystem();
    void init();
    void update(double dt);
    void render();

    static ClientConnectSystem* s_pCallbackInstance;  // Pointer to 'this' for CreateListenSocketIP callback

private:
    static void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void PollConnectionStateChanges();

    void SendStringToClient(HSteamNetConnection conn, const char* str);


    ComponentHandle<ServerSocketSingleton> server;
};
