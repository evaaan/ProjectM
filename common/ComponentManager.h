#pragma once
#include <array>
#include <memory>
#include <map>

#include "Entity.h"

#define MAX_NUMBER_OF_COMPONENTS 128

template<typename T>
struct Components
{
    unsigned int size = 1;
    std::unique_ptr<std::array<T, MAX_NUMBER_OF_COMPONENTS>> data;  // array of structs, allocated at runtime
};

struct ComponentInstance
{
    unsigned int index;
};

/* All managers inherit from this base class. This enables us to store different 
* ComponentManagers in the _one_ array. */
class BaseComponentManager {
public:
    BaseComponentManager() = default;
    virtual ~BaseComponentManager() = default;
    BaseComponentManager(const BaseComponentManager&) = default;             //  copy constructor
    BaseComponentManager& operator=(const BaseComponentManager&) = default;  //  normal copy
    BaseComponentManager(BaseComponentManager&&) = default;                  //  r-value constructor
    BaseComponentManager& operator=(BaseComponentManager&&) = default;       //  r-value copy
};

/* ComponentManager is a wrapper around a map of Entity : Component. */
template<typename T>
class ComponentManager : public BaseComponentManager
{
public:
    using LookupType = T;

    ComponentManager()
    {
        /* Whew. Allocate contiguous fixed array with size: sizeof(T) * MAX_NUMBER_OF_COMPONENTS */
        m_components.data = std::make_unique<std::array<T, MAX_NUMBER_OF_COMPONENTS>>(std::array<T, MAX_NUMBER_OF_COMPONENTS>{});
    }

    ComponentInstance add(Entity entity, T& component)
    {
        /* Add component */
        ComponentInstance newInstance = { m_components.size };
        m_components.data->at(m_components.size) = component;
        m_components.size++;

        /* Hold reference to this entity. Used later in destroy. */
        m_lastEntitySeen = entity;

        /* Update map */
        m_entityMap[entity] = newInstance;
        return newInstance;
    }

    /* Remove an item, and take the last item in the list and move it to fill the space. */
    void destroy(Entity entity)
    {
        /* Delete component */
        ComponentInstance deleteInst = m_entityMap[entity];                //  index of component to delete
        ComponentInstance lastInst = { m_components.size - 1 };                //  last index in array
        m_components.data->at(deleteInst.index) = m_components.data->at(lastInst.index);       //  delete component by overwriting with last component
        m_components.size--;

        /* Remove entity from map and update moved component */
        m_entityMap.erase(entity);
        m_entityMap[m_lastEntitySeen] = deleteInst;  //  Note: m_lastEntitySeen is set in add() to the last Entity in the array
    }

    /* Get number of Components */
    unsigned int size()
    {
        return m_components.size - 1;
    }

    /* Get pointer to Component data of type T for an Entity*/
    LookupType* lookup(Entity entity)
    {
        ComponentInstance instance = m_entityMap.at(entity);  // throws exception if entity doesn't exist

        /* Return address of component data */
        return &m_components.data->at(instance.index);
    }

    LookupType* getLastComponent()
    {
        return lookup(m_lastEntitySeen);
    }


    Entity getLastEntity()
    {
        return m_lastEntitySeen;
    }


private:
    Components<T> m_components;
    std::map<Entity, ComponentInstance> m_entityMap;  //  hash map of entity UUID to component index
    Entity m_lastEntitySeen;                                    //  last entity in m_components
};
