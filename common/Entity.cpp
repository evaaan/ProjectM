#include <memory>
#include "Entity.h"

EntityManager::EntityManager() {}

EntityManager::~EntityManager() {}

Entity EntityManager::create()
{
    lastEntity++;
    return { lastEntity };
}

/* Create Entity with the given id. Will overwrite any existing Entity.*/
Entity EntityManager::create(int uuid)
{
    // Increment lastEntity if we aren't overriding another Entity value
    if (uuid >= lastEntity)
        lastEntity = uuid + 1;
    return { uuid };
}

void EntityManager::destroy(Entity entity) {}
