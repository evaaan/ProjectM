#include <memory>
#include "pch.h"
#include "System.h"
#include "ComponentHandle.h"
#include "EntityHandle.h"
#include "Entity.h"
#include "World.h"

class SystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};
struct Position : Component<Position>
{
    Position() : m_x(0), m_y(0) {};
    Position(int x, int y) : m_x(x), m_y(y) {};
    int m_x;
    int m_y;
};

class Physics : public System
{
public:
    Physics()
    {
        signature.addComponent<Position>();
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
TEST_F(SystemTest, CreateSystem)
{
    std::unique_ptr<EntityManager> em = std::make_unique<EntityManager>();
    std::shared_ptr<World> world = std::make_shared<World>(std::move(em));
    std::unique_ptr<System> physics = std::make_unique<Physics>();
    world->addSystem(std::move(physics));

    EntityHandle player_entity = world->createEntity();
    player_entity.addComponent(Position(1, 1));

    for (int i = 0; i < 10; i++)
        world->update(10.0);
    
    auto c = player_entity.getComponent<Position>();
    EXPECT_EQ(c->m_x, 11);
    EXPECT_EQ(c->m_y, 11);
}
