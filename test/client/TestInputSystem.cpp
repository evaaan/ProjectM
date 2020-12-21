#include <array>
#include <memory>
#include "pch.h"
#include "../GameAppWin32/System.h"
#include "../GameAppWin32/ComponentHandle.h"
#include "../GameAppWin32/EntityHandle.h"
#include "../GameAppWin32/Entity.h"
#include "../GameAppWin32/World.h"
#include "../GameAppWin32/InputSystem.h"
#include "../GameAppWin32/InputManager.h"
#include "../GameAppWin32/Timer.h"
#include "../GameAppWin32/KeyMap.h"

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
        inputManager = std::make_unique<InputManager>(timer);
        inputSystem = std::make_unique<InputSystem>(inputManager.get());

        /* Add System, Entity, and Component to World */
        world->addSystem(std::move(inputSystem));
        input_entity = std::make_shared<EntityHandle>(world->createEntity());
        input_entity->addComponent(InputSingleton());  // 128 bitset
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
