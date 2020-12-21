#include <memory>
#include "pch.h"
#include "../GameAppWin32/System.h"
#include "../GameAppWin32/ComponentHandle.h"
#include "../GameAppWin32/EntityHandle.h"
#include "../GameAppWin32/Entity.h"
#include "../GameAppWin32/World.h"

class WorldTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        em = std::make_unique<EntityManager>();
        world = std::make_shared<World>(std::move(em));
    }


    std::unique_ptr<EntityManager> em;
    std::shared_ptr<World> world;
};
struct Position : Component<Position>
{
    Position() : m_x(0), m_y(0), init_called(false) {};
    Position(int x, int y) : m_x(x), m_y(y), init_called(false) {};
    int m_x;
    int m_y;
    bool init_called;
};

class PositionSystem : public System
{
public:
    PositionSystem()
    {
        signature.addComponent<Position>();
    }

    void init()
    {
        for (auto& entity : registeredEntities)
        {
            ComponentHandle<Position> position;
            parentWorld->unpack(entity, position);
            position->init_called = true;
        }
    }

    void update(double dt)
    {
        for (auto& entity : registeredEntities)
        {
            ComponentHandle<Position> position;
            parentWorld->unpack(entity, position);
            position->m_x += 1;  // increment 1 per update
            position->m_y += 1;
        }
    }
};

/* Add a system to the world and verify init is called */
TEST_F(WorldTest, WorldInit)
{
    std::unique_ptr<System> positionSystem = std::make_unique<PositionSystem>();
    world->addSystem(std::move(positionSystem));

    EntityHandle player_entity = world->createEntity();
    player_entity.addComponent(Position(1, 1));

    world->init();

    auto c = player_entity.getComponent<Position>();
    EXPECT_EQ(c->init_called, true);
}

/* Test add components */
TEST_F(WorldTest, AddDeleteComponents)
{
    std::vector<EntityHandle> handles;
    int num_to_add = 10;
    for (int i = 0; i < num_to_add; i++)
    {
        EntityHandle player_entity = world->createEntity();
        player_entity.addComponent(Position(1, 1));
        handles.push_back(player_entity);
    }
    EXPECT_EQ(world->getComponentManager<Position>()->size(), num_to_add);

    for (auto& entity : handles)
        entity.removeComponent<Position>();
    EXPECT_EQ(world->getComponentManager<Position>()->size(), 0);
}


/* Test Singleton Component */
TEST_F(WorldTest, SingletonComponent)
{
    EntityHandle player_entity = world->createEntity();
    player_entity.addSingletonComponent(Position(5, 10));
    player_entity.addSingletonComponent(Position(5, 10));
    EXPECT_EQ(world->getComponentManager<Position>()->size(), 1); // check we only added it once

    ComponentHandle<Position> singleton = world->getSingletonComponent<Position>();
    EXPECT_EQ(singleton->m_x, 5);
    EXPECT_EQ(singleton->m_y, 10);

    player_entity.removeComponent<Position>();
    EXPECT_EQ(world->getComponentManager<Position>()->size(), 0); // test delete

}
