#include <array>
#include "pch.h"
#include "Component.h"
#include "ComponentManager.h"
#include "Entity.h"

class ComponentTest : public ::testing::Test
{
protected:
    struct Health : public Component<Health>
    {
        unsigned int hp;
        unsigned int max_hp;
    };
    struct Position : public Component<Position>
    {
        unsigned int x = 0;
        unsigned int y = 0;
    };
    struct Gravity : public Component<Gravity>
    {
        float accel_z = -9.81;
    };


    void SetUp() override
    {
    }
};

/* Add multiple components and check their indices. */
TEST_F(ComponentTest, AddComponent)
{
    ComponentManager<Health> cm;
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    Health h1;
    Health h2;
    Health h3;
    ComponentInstance inst1 = cm.add(e1, h1);
    ComponentInstance inst2 = cm.add(e2, h2);
    ComponentInstance inst3 = cm.add(e3, h3);
    
    ASSERT_EQ(inst1.index, 1);
    ASSERT_EQ(inst2.index, 2);
    ASSERT_EQ(inst3.index, 3);
}

/* Add components, then look them up it up.*/
TEST_F(ComponentTest, LookupComponent)
{
    ComponentManager<Health> cm;
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    Health h1, h2, h3;
    h1.hp = 100;
    h1.max_hp = 150;
    h2.hp = 200;
    h2.max_hp = 250;
    h3.hp = 300;
    h3.max_hp = 350;
    cm.add(e1, h1);
    cm.add(e2, h2);
    cm.add(e3, h3);

    Health* h_ptr3 = cm.lookup(e3);
    Health* h_ptr2 = cm.lookup(e2);
    Health* h_ptr1 = cm.lookup(e1);
    ASSERT_EQ(h_ptr1->hp, 100);
    ASSERT_EQ(h_ptr1->max_hp, 150);
    ASSERT_EQ(h_ptr2->hp, 200);
    ASSERT_EQ(h_ptr2->max_hp, 250);
    ASSERT_EQ(h_ptr3->hp, 300);
    ASSERT_EQ(h_ptr3->max_hp, 350);
}

/* Add components, then destroy one and check the array re-sizes itself*/
TEST_F(ComponentTest, SimpleDestroyComponent)
{
    ComponentManager<Health> cm;
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    Health h1, h2, h3;
    h1.hp = 100;
    h1.max_hp = 150;
    h2.hp = 200;
    h2.max_hp = 250;
    h3.hp = 300;
    h3.max_hp = 350;

    cm.add(e1, h1);
    cm.add(e2, h2);
    cm.add(e3, h3);
    ASSERT_EQ(cm.size(), 3);

    cm.destroy(e1); // moves e3 to e1's position
    ASSERT_EQ(cm.size(), 2);         // Check array size decreases

    Health* h_ptr1 = cm.lookup(e3);  // verify e3 is re-assigned in m_entityMap
    ASSERT_EQ(h_ptr1->hp, 300);
    ASSERT_EQ(h_ptr1->max_hp, 350);


}

/* Add components, and destroy them */
TEST_F(ComponentTest, ComplexDestroyComponent)
{
    ComponentManager<Health> cm;
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    Entity e4 = em.create();
    Entity e5 = em.create();
    Health h1, h2, h3, h4, h5;
    h4.hp = 200;
    h4.max_hp = 250;
    h5.hp = 300;
    h5.max_hp = 350;

    cm.add(e1, h1);
    cm.add(e2, h2);
    cm.add(e3, h3);
    cm.add(e4, h4);
    cm.add(e5, h5);
    ASSERT_EQ(cm.size(), 5);

    //  1, 2, 3, 4, 5
    cm.destroy(e3); //  1, 2, 5, 4
    cm.destroy(e1); //  4, 2, 5
    cm.destroy(e2); //  4, 5
    ASSERT_EQ(cm.size(), 2);

    Health* h_ptr4 = cm.lookup(e4);  // verify e3 is re-assigned in m_entityMap
    Health* h_ptr5 = cm.lookup(e5);  // verify e3 is re-assigned in m_entityMap
    ASSERT_EQ(h_ptr4->hp, 200);
    ASSERT_EQ(h_ptr4->max_hp, 250);
    ASSERT_EQ(h_ptr5->hp, 300);
    ASSERT_EQ(h_ptr5->max_hp, 350);
}

/* Add many components */
TEST_F(ComponentTest, AddManyComponents)
{
    const int num_components = MAX_NUMBER_OF_COMPONENTS - 1;
    ComponentManager<Health> cm;
    EntityManager em;
    Entity e;

    std::array<Health, num_components> components;
    std::vector<Entity> entities;

    for (auto& c : components)
    {
        e = em.create();
        entities.push_back(e);
        cm.add(e, c);
    }
    ASSERT_EQ(cm.size(), num_components);

    // Destroy front to back
    for (auto& e : entities)
        cm.destroy(e);


    ASSERT_EQ(cm.size(), 0);
}

/* Test component family */
TEST_F(ComponentTest, ComponentFamilyIncrements)
{
    ComponentManager<Health> cm_h;
    ComponentManager<Position> cm_p;
    ComponentManager<Gravity> cm_g;
    EntityManager em;
    Entity e1 = em.create();
    Entity e2 = em.create();
    Entity e3 = em.create();
    Health h1, h2, h3;
    Position p1, p2;
    Gravity g1, g2;
    cm_h.add(e1, h1);  // Health = 0
    cm_p.add(e1, p1);  // Position = 1
    cm_g.add(e1, g1);  // Gravity = 2
    
    // Check component family doesn't change when adding more components.
    cm_h.add(e2, h2);
    cm_g.add(e2, g2);
    cm_p.add(e2, p2);
    cm_h.add(e3, h3);


    ASSERT_EQ(GetComponentFamily<Health>(), 0);
    ASSERT_EQ(GetComponentFamily<Position>(), 1);
    ASSERT_EQ(GetComponentFamily<Gravity>(), 2);
}
