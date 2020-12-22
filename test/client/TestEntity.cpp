#include "pch.h"
#include "Entity.h"

class EntityTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};

TEST_F(EntityTest, CreateEntities)
{
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    EXPECT_EQ(e1.uuid, 1);
    EXPECT_EQ(e2.uuid, 2);
    EXPECT_EQ(e3.uuid, 3);
}

TEST_F(EntityTest, EntityBoolOperator)
{
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    ASSERT_TRUE(e1 < e2);
}
