#include <iostream>
#include "Timer.h"
#include "World.h"
#include "EntityHandle.h"
#include "World.h"
#include "ServerManager.h"
#include "Timer.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "System.h"
#include "ClientConnectSystem.h"
#include "ClientInputSystem.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PlayerSystem.h"
#include "ClientUpdateSystem.h"
#include "Utilities.h"

ServerManager::ServerManager(int port_num, int tick_ms) :
    m_port(port_num),
    m_tick(tick_ms),
    m_world(nullptr)
{

    /* Create and start the timer */
    m_timer = std::make_shared<Timer>();
    m_timer->start();
}


ServerManager::~ServerManager()
{
}

void ServerManager::Init()
{
    std::unique_ptr<EntityManager> em = std::make_unique<EntityManager>();
    m_world = std::make_shared<World>(std::move(em));  // Systems have shared ownership of World

    /* Add game systems */
    AddSystems();

    /* Add game entities */
    AddEntities();

    /* Initialize the World */
    m_world->init();
}

/* Server runs in 3 steps:
*  1. Reset RecentUpdate flags
*  1. Read inputs from clients (key presses)
*  2. Process world tick (bulk of systems run here). If an Entity component changed, set RecentUpdate flag
*  3. Serialize and send Entity Components with RecentUpdate set to all clients
* 
* All client-server communication is done over SteamNetworkingSockets. 
* Clients map their keypresses into actions (such as "move left"), encode it
* in a 128-bit map, and send it to the client every tick. On the server, the
* ClientInputSystem reads the bitmap from the 'Steam Socket' and stores it in that
* entity's InputState component. This component is accessible by any system
* that needs to know the client's inputs.
* 
* When Systems change an Entity, the system MUST set the touched Component's RecentUpdate flag.
* The ClientUpdateSystem will serialize and send these Components to every client.
*/
void ServerManager::AddSystems()
{
    /* Maintain client connections */
    m_world->addSystem(std::move(std::make_unique<ClientConnectSystem>()));

    /* Read and parse client messages */
    m_world->addSystem(std::move(std::make_unique<ClientInputSystem>()));

    m_world->addSystem(std::move(std::make_unique<PhysicsSystem>()));
    //m_world->addSystem(std::move(std::make_unique<CombatSystem>()));
    //m_world->addSystem(std::move(std::make_unique<PlayerSystem>()));

    /* Send entity updates to clients */
    m_world->addSystem(std::move(std::make_unique<ClientUpdateSystem>()));
}

int ServerManager::AddBox(int x, int y, int height, int width, Color color)
{
    // Create entity
    auto box = m_world->createEntity();
    box.addComponent(Transform());
    box.addComponent(Dynamic());
    box.addComponent(Outline());
    auto transform = box.getComponent<Transform>();
    transform->x = x;
    transform->y = y;
    transform->width = width;
    transform->height = height;

    auto dynamic = box.getComponent<Dynamic>();
    dynamic->width = width;
    dynamic->height = height;
    dynamic->pos.x = x;
    dynamic->prev_pos.x = x;
    dynamic->pos.y = y;
    dynamic->prev_pos.y = y;
    dynamic->vel.x = 0;
    dynamic->vel.y = 0;
    dynamic->accel.x = 0;
    dynamic->accel.y = 0;
    dynamic->type = BodyType::Ledge;
    dynamic->falling = false;

    auto outline = box.getComponent<Outline>();
    outline->width = 2;
    outline->color = color;

    // Update worldDelta
    auto worldDelta = m_world->getSingletonComponent<WorldDeltaSingleton>();
    int entity_id = box.id();
    worldDelta->state[entity_id] = ComponentMask();
    worldDelta->state[entity_id].addDefaultComponent<Transform, Dynamic, Outline>();
    worldDelta->state[entity_id].setDefault();

    odsloga("Created box with id " << entity_id << "\n");

    return entity_id;
}

void ServerManager::AddEntities()
{
    // Store all singleton components in a single entity
    auto e = m_world->createEntity();
    e.addSingletonComponent(CollisionSingleton()); // Collision Data
    e.addSingletonComponent(ServerSocketSingleton());  // Client Connection Data
    e.addSingletonComponent(WorldDeltaSingleton()); //  Store Entity changes each tick
    e.addSingletonComponent(KeyStateSingleton());  // User input

    // Create boxes and update worldDelta
    // int x, int y, int height, int width, Color color
    AddBox(200, 650, 50, 300, Color::red);
    AddBox(900, 650, 50, 200, Color::red);
    AddBox(0, 700, 6000, 2000, Color::red);
}

long numFrames = 0;

void printFrameInfo(double totalTime, double deltaTime, double updateTime)
{
    // static long numFrames = 0;
    static double lastUpdate = 0;
    if (totalTime - lastUpdate > 1.0)
    {
        lastUpdate = totalTime;
        std::cout << "Frame num: " << numFrames;

        std::cout << ", update: " << std::fixed << updateTime * 1000.0 << " ms";
        
        double usTime = deltaTime * 1000.0;
        std::cout << ", tick: " << std::fixed << usTime << " ms" << std::endl;
    }
}

/* Tick the world */
void ServerManager::Process()
{
    double startTime;
    double updateTime;

    /* Tick the timer */
    numFrames++;
    m_timer->tick();

    /* Update the world */
    startTime = m_timer->getCurrentTime();
    m_world->update(m_timer->getDeltaTime());
    updateTime = m_timer->getCurrentTime() - startTime;

    /* Print debug info */
    m_timer->sleep_until(m_tick); // ms
    printFrameInfo(m_timer->getTotalTime(), m_timer->getDeltaTime(), updateTime);
}

/* Shut it down */
void ServerManager::Shutdown()
{
}

World* ServerManager::getWorld()
{
    return m_world.get();
}