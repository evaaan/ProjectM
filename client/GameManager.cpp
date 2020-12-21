#include "EntityHandle.h"
#include "App.h"
#include "World.h"
#include "InputManager.h"
#include "GraphicManager.h"
#include "GameManager.h"
#include "Timer.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "System.h"
#include "AnimationSystem.h"
#include "InputSystem.h"
#include "PhysicsSystem.h"
#include "OverlaySystem.h"
#include "ImGuiSystem.h"

GameManager::GameManager(std::shared_ptr<Timer> timer, InputManager* input, GraphicManager* graphic) :
    m_timer(timer), m_inputManager(input), m_graphicManager(graphic), m_world(nullptr) {}

GameManager::~GameManager() {}

/* Create Entities, Components, and Systems, then initialize the World. */
void GameManager::Init()
{
    // World
    std::unique_ptr<EntityManager> em = std::make_unique<EntityManager>();
    m_world = std::make_shared<World>(std::move(em));  // Systems must have shared ownership of the World

    /* Add game systems */
    AddSystems();

    /* Initialize the World */
    m_world->init();

    /* Pass the World reference to GraphicManager to render it*/
    m_graphicManager->registerWorld(m_world.get());
}

void GameManager::AddSystems()
{
    // Systems
    m_world->addSystem(std::move(std::make_unique<ImGuiSystem>(m_graphicManager, m_world.get())));
    m_world->addSystem(std::move(std::make_unique<InputSystem>(m_inputManager)));
    m_world->addSystem(std::move(std::make_unique<AnimationSystem>(m_graphicManager)));
    m_world->addSystem(std::move(std::make_unique<OverlaySystem>(m_graphicManager)));
    m_world->addSystem(std::move(std::make_unique<PhysicsSystem>()));

}

/* Tick the world */
void GameManager::Update()
{
	// tick timer
    m_timer->tick();

	// Update world
    m_world->update(m_timer->getDeltaTime());
}

/* Get a pointer to the current world */
World* GameManager::getWorld()
{
    return m_world.get();
}