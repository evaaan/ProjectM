#pragma once

// Forward declarations
class System;
struct ServerSocketSingleton;
struct WorldDeltaSingleton;
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
    void sendWorldUpdate();
    void sendPlayerUpdates();

    ComponentHandle<ServerSocketSingleton> server;
    ComponentHandle<WorldDeltaSingleton> worldDelta;

};
