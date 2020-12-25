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
#include "flatbuffers/flatbuffers.h"
#include "steam/steamnetworkingtypes.h"

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

void ServerManager::AddSystems()
{

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