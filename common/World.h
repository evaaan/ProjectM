#pragma once 
#include <map>
#include <memory>
#include <vector>

#include "ComponentManager.h"
#include "ComponentHandle.h"
#include "ComponentMask.h"

// Forward declarations
struct Entity;
class EntityManager;
struct EntityHandle;
class System;

class World : public std::enable_shared_from_this<World>  // Inherit publicly from shared_from_this
{
public:

    World(std::unique_ptr<EntityManager> em);
    ~World();

    /* Called on engine init */
    void init();

    /* Create a new Entity */
    EntityHandle createEntity();

    /* Destroy an Entity and all its Components*/
    void destroyEntity(Entity e);

    /* Return all entity IDs registered in world */
    std::set<int> getEntityIds();

    /* Add a Component associated with an Entity */
    template <typename T>
    void addComponent(Entity const& entity, T&& component) {
        ComponentManager<T>* manager = getComponentManager<T>();  // add ComponentManager
        manager->add(entity, component);

        ComponentMask oldMask = m_entityMasks[entity];
        m_entityMasks[entity].addComponent<T>();

        updateEntityMask(entity, oldMask);
    }

    /* Remove Component Type T for the given Entity. Note: Entities can only have 1 of each Component type. */
    template <typename T>
    void removeComponent(Entity const& entity) {
        ComponentManager<T>* manager = getComponentManager<T>();
        manager->destroy(entity);
        // auto manager = getComponentManager<T>();
        // ComponentHandle<T> component = manager->lookup(entity);
        // component.destroy();

        ComponentMask oldMask = m_entityMasks[entity];
        m_entityMasks[entity].removeComponent<T>();

        updateEntityMask(entity, oldMask);
    }

    /* Add a new system */
    void addSystem(std::unique_ptr<System> system);

    /* Load initial world state */
    void load();

    /* Tick world behavior */
    void update(double dt);

    /* Render world */
    void render();

    /* Create entities */
    void addWolf(int x, int y);
    void addBox(int x, int y);

    /* Unpack Entity and Components from the World*/
    template <typename ComponentType, typename... Args>
    void unpack(Entity e, ComponentHandle<ComponentType>& handle, ComponentHandle<Args> &... args) {
        typedef ComponentManager<ComponentType> ComponentManagerType;
        auto mgr = getComponentManager<ComponentType>();
        handle = ComponentHandle<ComponentType>(e, mgr->lookup(e), mgr);

        // Recurse
        unpack<Args...>(e, args...);
    }

    /* Base recursive case */
    template <typename ComponentType>
    void unpack(Entity e, ComponentHandle<ComponentType>& handle) {
        typedef ComponentManager<ComponentType> ComponentManagerType;
        auto mgr = getComponentManager<ComponentType>();
        handle = ComponentHandle<ComponentType>(e, mgr->lookup(e), mgr);
    }

    /* Return ComponentManager for type T */
    template <typename T>
    ComponentManager<T>* getComponentManager() {
        // Need to make sure we actually have a component manager.
        int family = GetComponentFamily<T>();

        if (family >= m_componentManagers.size()) {
            m_componentManagers.resize(family + 1);
        }

        /* Create ComponentManager if it does not exist */
        if (!m_componentManagers[family]) {
            m_componentManagers[family] = std::make_unique<ComponentManager<T>>();
        }

        /* Return a raw pointer instead of shared_ptr because we return a derived class.
        Using shared_ptr would cause the object that calls this function to delete the ComponentManager
        when the shared_ptr goes out of scope. */
        return static_cast<ComponentManager<T>*>(m_componentManagers[family].get());
    }

    /*What is a SingletonComponent?
    * The same as a Component, with the following rules:
    *   - Only 1 SingletonComponent may exist at a time. It's added with addSingletonComponent()
    *   - SingletonComponents are accessible in the World with getSingletonComponent()
    * 
    * Use them as a shared data Component accessible from ALL systems. For example, user input.
    * SingletonComponents should be written to by few systems, but read by many systems.
    */
    template <typename T>
    ComponentHandle<T> getSingletonComponent()
    {
        auto mgr = getComponentManager<T>();
        return ComponentHandle<T>(mgr->getLastEntity(), mgr->getLastComponent(), mgr);
    }

    /* Add a Singleton Component associated with an Entity.
    Same as addComponent, but only add a maximum of 1 component to the manager. */
    template <typename T>
    void addSingletonComponent(Entity const& entity, T&& component) {
        ComponentManager<T>* manager = getComponentManager<T>();  // add ComponentManager
        if (manager->size() == 0)
        {
            manager->add(entity, component);
            ComponentMask oldMask = m_entityMasks[entity];
            m_entityMasks[entity].addComponent<T>();
            updateEntityMask(entity, oldMask);
        }
    }

private:
    std::vector<std::unique_ptr<System>> m_systems;                          //  World behaviors
    std::unique_ptr<EntityManager> m_entityManager;                          //  Wrapper to create/delete Entities
    std::vector<std::unique_ptr<BaseComponentManager>> m_componentManagers;  //  World states
    std::map<Entity, ComponentMask> m_entityMasks;                           // The Component Types that belong to each Entity
    std::set<int> m_entitySet;                                                // All entities registered in the world
    void updateEntityMask(Entity const& entity, ComponentMask oldMask);
};
