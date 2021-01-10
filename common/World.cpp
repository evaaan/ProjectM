#include "Entity.h"
#include "EntityHandle.h"
#include "System.h"
#include "World.h"

enum class Color { yellow, red, green, blue, black, white };

World::World(std::unique_ptr<EntityManager> em) : m_entityManager(std::move(em))
{
}

World::~World() {}

void World::addWolf(int x, int y)
{
    EntityHandle wolf_entity = createEntity();
    wolf_entity.addComponent(Animation());
    wolf_entity.addComponent(Transform());
    wolf_entity.addComponent(Dynamic());
    wolf_entity.addComponent(Outline());

    auto transform = wolf_entity.getComponent<Transform>();
    transform->width = 64;
    transform->height = 64;

    auto dynamic = wolf_entity.getComponent<Dynamic>();
    dynamic->width = transform->width;
    dynamic->height = transform->height;
    dynamic->pos.x = x;
    dynamic->pos.y = y;
    dynamic->vel.x = 0.0;
    dynamic->vel.y = 0.0;
    dynamic->accel.x = 0.0;
    dynamic->accel.y = 3000.0;
    dynamic->type = BodyType::Player;
}

void World::addBox(int x, int y)
{
    // Box
    EntityHandle solid_entity = createEntity();
    solid_entity.addComponent(Transform());
    solid_entity.addComponent(Dynamic());
    solid_entity.addComponent(Outline());
    auto solid_transform = solid_entity.getComponent<Transform>();
    solid_transform->width = 200;
    solid_transform->height = 200;
    auto solid_dynamic = solid_entity.getComponent<Dynamic>();
    solid_dynamic->width = solid_transform->width;
    solid_dynamic->height = solid_transform->height;
    solid_dynamic->pos.x = x;
    solid_dynamic->pos.y = y;
    solid_dynamic->type = BodyType::Ledge;
    auto solid_outline = solid_entity.getComponent<Outline>();
    solid_outline->color = Color::red;
    solid_outline->width = 1;
}

void World::init()
{
    /* Load world state (entities and components) */
    load();

    /* Initialize Systems */
    for (auto& system : m_systems)
        system->init();
}

/* Load initial world state. Hard coded for now. */
void World::load()
{    

    addWolf(400, 400);
    addBox(400, 900);
    addBox(400, 600);
    addBox(700, 600);
    addBox(1000, 500);
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

/* Destroy an Entity */
void World::destroyEntity(Entity entity)
{
    for (auto& system : m_systems)
        system->unRegisterEntity(entity);
    m_entitySet.erase(entity.uuid);
    m_entityManager->destroy(entity);
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


