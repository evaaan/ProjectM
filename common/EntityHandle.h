#pragma once
#include <memory>
#include <utility>
#include "Entity.h"
#include "World.h"

// Forward Declarations
class World;

template <typename ComponentType>
struct ComponentHandle;

/* Syntax helpers around a wrapper to World. Example:
* EntityHandle player = world.createEntity();
* player.addComponent(std::make_unique<ArmorComponent>());
* player.addComponent(std::make_unique<HealthComponent>());
*/
struct EntityHandle
{
    Entity entity;
    std::shared_ptr<World> world;

    EntityHandle(Entity e, std::shared_ptr<World> w) : entity(e), world(w) {};

    int id()
    {
        return entity.uuid;
    }

    void destroy()
    {
        world->destroyEntity(entity);
    }

    // Use variadic template to accept any number of Components
    template <typename ComponentType, typename... Args>
    void addComponent(ComponentType&& component, Args&&... args)
    {
        world->addComponent<ComponentType>(entity, std::forward<ComponentType>(component));

        // Recurse
        addComponent<Args...>(std::forward<Args>(args)...);
    }

    template <typename ComponentType>
    void addComponent(ComponentType&& component)
    {
        world->addComponent<ComponentType>(entity, std::forward<ComponentType>(component));
    }


    template <typename ComponentType>
    void addSingletonComponent(ComponentType&& component)
    {
        world->addSingletonComponent<ComponentType>(entity, std::forward<ComponentType>(component));
    }

    template <typename ComponentType>
    void removeComponent()
    {
        world->removeComponent<ComponentType>(entity);
    }

    template <typename ComponentType>
    ComponentHandle<ComponentType> getComponent()
    {
        ComponentHandle<ComponentType> handle;
        world->unpack(entity, handle);
        return handle;
    }

};
