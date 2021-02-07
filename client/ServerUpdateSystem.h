#pragma once

// Forward declarations
class System;
class EntityBuffer::Entity;

class ServerUpdateSystem : public System {
public:
    ServerUpdateSystem();
    void init();
    void update(double dt);
    void render();

private:
    ComponentHandle<ClientSocketSingleton> client;

    void processFbMessage(uint8_t* buf_ptr);
    void createEntity(const EntityBuffer::Entity* entity_buffer);
    void updateEntity(const EntityBuffer::Entity* entity_buffer);
};
