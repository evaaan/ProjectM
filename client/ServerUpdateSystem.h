#pragma once
#include "fbs/entity_generated.h"

// Forward declarations
class GraphicManager;

class ServerUpdateSystem : public System {
public:
    ServerUpdateSystem(GraphicManager* graphicManager);
    void init();
    void update(double dt);
    void render();

private:
    ComponentHandle<ClientSocketSingleton> client;

    void processFbMessage(uint8_t* buf_ptr);
    void createEntity(const EntityBuffer::Entity* entity_buffer);
    void updateEntity(const EntityBuffer::Entity* entity_buffer);
    GraphicManager* m_graphicManager;
};
