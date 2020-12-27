#include <iostream>
#include "flatbuffers/flatbuffers.h"
#include "steam/steamnetworkingtypes.h"
#include "Timer.h"
#include "World.h"
#include "EntityHandle.h"
#include "World.h"
#include "ServerManager.h"
#include "Timer.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "System.h"
#include "ClientInputSystem.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PlayerSystem.h"
#include "UpdateClientSystem.h"

ServerManager::ServerManager(std::shared_ptr<Timer> timer) :
    m_timer(timer),
    m_world(nullptr)
{
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
* ClientInputSystem reads the bitmap from the socket and stores it in that
* entity's InputState component. This component is accessible by any system
* that needs to know the client's inputs.
* 
* When Systems change an Entity, the system MUST set the touched Component's RecentUpdate flag.
* The UpdateClientSystem will serialize and send these Components to every client.
*/
void ServerManager::AddSystems()
{
    // m_world->addSystem(std::move(std::make_unique<System>()));
    m_world->addSystem(std::move(std::make_unique<ClientInputSystem>()));
    m_world->addSystem(std::move(std::make_unique<PhysicsSystem>()));
    m_world->addSystem(std::move(std::make_unique<PlayerSystem>()));
    m_world->addSystem(std::move(std::make_unique<CombatSystem>()));
    m_world->addSystem(std::move(std::make_unique<UpdateClientSystem>()));
}


void printFrameInfo(double totalTime, double deltaTime)
{
    static long numFrames = 0;
    static double lastUpdate = 0;
    if (totalTime - lastUpdate > 0.5)
    {
        lastUpdate = totalTime;
        std::cout << "Frame num: " << numFrames++;
        
        double usTime = deltaTime * 1000.0;
        std::cout << ", time: " << std::fixed << usTime << " ms" << std::endl;
    }

    numFrames++;
}

/* Tick the world */
void ServerManager::Process()
{
    m_timer->tick();
    m_world->update(m_timer->getDeltaTime());
    printFrameInfo(m_timer->getTotalTime(), m_timer->getDeltaTime());
}

World* ServerManager::getWorld()
{
    return m_world.get();
}