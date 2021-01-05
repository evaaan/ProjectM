#pragma once

// Forward declarations
class System;

class ServerInputSystem : public System {
public:
    ServerInputSystem();
    void init();
    void update(double dt);
    void render();

    void updateServer();
};
