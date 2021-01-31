#include "Entity.h"
#include "EntityHandle.h"
#include "System.h"
#include "World.h"

enum class Color { yellow, red, green, blue, black, white };

World::World(std::unique_ptr<EntityManager> em) : m_entityManager(std::move(em))
{
}

World::~World() {}

void World::init()
{
    /* Initialize Systems */
    for (auto& system : m_systems)
        system->init();
}


/* Update Systems */
void World::update(double dt)
{
    for (auto& system : m_systems)
        system->update(dt);
}

/* Render Systems */
void World::render()
{
    for (auto& system : m_systems)
        system->render();
}

/* Create an Entity */
EntityHandle World::createEntity()
{
    auto entity = m_entityManager->create();
    m_entitySet.insert(entity.uuid);
    return { entity, shared_from_this() };
}

/* Create an Entity with given id. Override any Entity that already exists with this id. */
EntityHandle World::createEntity(int id)
{
    if (entityExists({ id }))
        destroyEntity({ id });
    auto entity = m_entityManager->create(id);
    m_entitySet.insert(entity.uuid);
    return { entity, shared_from_this() };
}


/* Destroy an Entity */
void World::destroyEntity(Entity entity)
{
    for (auto& system : m_systems)
        system->unRegisterEntity(entity);
    m_entitySet.erase(entity.uuid);
    m_entityManager->destroy(entity);
}

/* Test if Entity exists */
bool World::entityExists(Entity entity)
{
    return m_entitySet.find(entity.uuid) != m_entitySet.end();

}

std::set<int> World::getEntityIds()
{
    return m_entitySet;
}

/* Add a System*/
void World::addSystem(std::unique_ptr<System> system)
{
    system->registerWorld(shared_from_this());
    m_systems.push_back(std::move(system));
}

/* Update the ComponentMask used by Systems to identify unique tuples */
void World::updateEntityMask(Entity const& entity, ComponentMask oldMask)
{
    ComponentMask newMask = m_entityMasks[entity];

    for (auto& system : m_systems)
    {
        ComponentMask systemSignature = system->getSignature();
        if (newMask.isNewMatch(oldMask, systemSignature))
            system->registerEntity(entity);  // We match but didn't before
        else if (newMask.isNoLongerMatched(oldMask, systemSignature))
            system->unRegisterEntity(entity);
    }
}


