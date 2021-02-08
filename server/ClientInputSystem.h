#pragma once

// Forward declarations
class ClientInputSystem;
class Entity;

class ClientInputSystem : public System {
public:
    ClientInputSystem();
    void init();
    void update(double dt);
    void render();

private:
    void processInputMessage(const EntityBuffer::Entity* entity_buffer, Entity e);
    ComponentHandle<ServerSocketSingleton> server;
};
