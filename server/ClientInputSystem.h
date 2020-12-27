#pragma once

// Forward declarations
class ClientInputSystem;

class ClientInputSystem : public System {
public:
    ClientInputSystem();
    void init();
    void update(double dt);
    void render();
};
