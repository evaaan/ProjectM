#include <array>
#include <memory>
#include "pch.h"
#include "System.h"
#include "ComponentHandle.h"
#include "EntityHandle.h"
#include "Entity.h"
#include "World.h"
#include "InputSystem.h"
#include "InputManager.h"
#include "Timer.h"
#include "KeyMap.h"

class InputSystemTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        /* Create World */
        em = std::make_unique<EntityManager>();
        world = std::make_shared<World>(std::move(em));

        /* Build InputManager and pass to InputSystem */
        timer = std::make_shared<Timer>();
        auto isActive = true;
        inputManager = std::make_unique<InputManager>(timer, isActive);
        inputSystem = std::make_unique<InputSystem>(inputManager.get());

        /* Add System, Entity, and Component to World */
        world->addSystem(std::move(inputSystem));
        input_entity = std::make_shared<EntityHandle>(world->createEntity());
        input_entity->addComponent(KeyState());  // 128 bitset
        world->init();
    }

    std::unique_ptr<EntityManager> em;
    std::shared_ptr<World> world;
    std::shared_ptr<Timer> timer;
    std::unique_ptr<InputManager> inputManager;
    std::unique_ptr<System> inputSystem;
    std::shared_ptr<EntityHandle> input_entity;
};

/* Runtime check */
TEST_F(InputSystemTest, InitInputSystem)
{
}
