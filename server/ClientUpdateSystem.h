#pragma once

// Forward declarations
class System;
struct ServerSocketSingleton;
template <typename T>
struct ComponentHandle;

class ClientUpdateSystem : public System {
public:
    ClientUpdateSystem();
    void init();
    void update(double dt);
    void render();

private:
    void UpdateAllClients();


    ComponentHandle<ServerSocketSingleton> server;

};
