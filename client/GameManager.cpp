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
#include "ServerConnectSystem.h"
#include "Direct2D.h"

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

    /* Add game entities */
    AddEntities();

    /* World initializes all systems */
    m_world->init();

    /* Pass the World reference to GraphicManager to render it*/
    m_graphicManager->registerWorld(m_world.get());

}

void GameManager::AddSystems()
{
    m_world->addSystem(std::move(std::make_unique<ImGuiSystem>(m_graphicManager)));
    const char* serverAddr = "127.0.0.1:35656";
    m_world->addSystem(std::move(std::make_unique<ServerConnectSystem>(serverAddr)));

    m_world->addSystem(std::move(std::make_unique<InputSystem>(m_inputManager)));
    m_world->addSystem(std::move(std::make_unique<PhysicsSystem>()));
    m_world->addSystem(std::move(std::make_unique<AnimationSystem>(m_graphicManager)));
    m_world->addSystem(std::move(std::make_unique<OverlaySystem>(m_graphicManager)));


}

void GameManager::AddEntities()
{
    m_world->createEntity().addSingletonComponent(InputSingleton()); // Input
    m_world->createEntity().addSingletonComponent(CollisionSingleton()); // Collision Data
    m_world->createEntity().addSingletonComponent(ClientSocketSingleton());  // Client Connection Data

    addWolf(400, 400);
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

void GameManager::addWolf(int x, int y)
{
    EntityHandle wolf_entity = m_world->createEntity();
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

void GameManager::addBox(int x, int y)
{
    // Box
    EntityHandle solid_entity = m_world->createEntity();
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