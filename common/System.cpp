#include "World.h"
#include "Entity.h"
#include "System.h"

void System::registerWorld(std::shared_ptr<World> world)
{
    parentWorld = world;
}

void System::registerEntity(Entity const& entity)
{
    registeredEntities.push_back(entity);
}

void System::unRegisterEntity(Entity const& entity)
{
    for (auto it = registeredEntities.begin(); it != registeredEntities.end(); ++it)
    {
        Entity e = *it;
        if (e.uuid == entity.uuid)
        {
            registeredEntities.erase(it);
            return;
        }
    }
}

ComponentMask System::getSignature()
{
    return signature;
}
