#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
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
    void processInputMessage(const EntityBuffer::Entity* entity_buffer, Entity e, boost::uuids::uuid uuid);
    ComponentHandle<ServerSocketSingleton> server;
    ComponentHandle<KeyStateSingleton> keys;
};
