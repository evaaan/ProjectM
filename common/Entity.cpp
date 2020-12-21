#include <memory>
#include "Entity.h"

EntityManager::EntityManager() {}

EntityManager::~EntityManager() {}

Entity EntityManager::create()
{
    lastEntity++;
    return { lastEntity };
}

void EntityManager::destroy(Entity entity) {}
