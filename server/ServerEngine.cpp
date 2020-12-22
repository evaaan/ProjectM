#include "ServerEngine.h"
#include "ServerManager.h"
#include "Timer.h"

using namespace ServerControl;

ServerEngine::ServerEngine() : m_engineState(EngineState::Startup)
{
    /* Create and start the timer */
    m_timer = std::make_shared<Timer>();
    m_timer->start();

    m_server = std::make_unique<ServerManager>(m_timer);
}

ServerEngine::~ServerEngine()
{
}

void ServerEngine::Init()
{
}

void ServerEngine::Run()
{
    while (m_engineState != EngineState::Terminate)
    {
        switch (m_engineState)
        {
            case EngineState::Startup:
            {
                /* Initialize worlds */
                m_server->Init();

                m_engineState = EngineState::Active;
                break;
            }

            case EngineState::Active:
            {
                /* Read user input */
                m_server->ReadClients();
                m_server->Process();
                m_server->UpdateClients();
                break;
            }
        }
    }
}

void ServerEngine::Shutdown()
{
}
