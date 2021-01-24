#pragma once

// Forward declarations
class System;

class ServerUpdateSystem : public System {
public:
    ServerUpdateSystem();
    void init();
    void update(double dt);
    void render();

private:
    ComponentHandle<ClientSocketSingleton> client;

    void processFbMessage(uint8_t* buf_ptr);
};
