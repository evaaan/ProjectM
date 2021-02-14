#pragma once
#include "fbs/entity_generated.h"

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
    void processInputMessage(const EntityBuffer::Entity* entity_buffer, int id);  // Input message and entity ID
    ComponentHandle<ServerSocketSingleton> server;
    ComponentHandle<KeyStateSingleton> keys;
};
