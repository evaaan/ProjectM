#pragma once

// Forward declarations
class System;
struct ClientSocketSingleton;
template <typename T>
struct ComponentHandle;

class ServerInputSystem : public System {
public:
    ServerInputSystem(const char *server_addr);  // IPv4 (or IPv6?) server address
    ~ServerInputSystem();
    void init();
    void update(double dt);
    void render();

    static ServerInputSystem* s_pCallbackInstance;  // Pointer to 'this' for CreateListenSocketIP callback


private:
    static void ConnStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void OnConnStatusChange(SteamNetConnectionStatusChangedCallback_t* pInfo);
    void PollConnectionStateChanges();
    void UpdateServer();

    ComponentHandle<ClientSocketSingleton> client;
    const char* m_server_addr;
};
