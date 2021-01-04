#include <windows.h>
#include <stdio.h>
#include "ServerEngine.h"
#include "ServerManager.h"
#include "Timer.h"
#include "Utilities.h"

using namespace ServerControl;

ServerControl::EngineState engineState = EngineState::Startup;


/* Handle Ctrl-C signal and CTRL-CLOSE (console exit) gracefully on Windows */
BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
        printf("Ctrl-C event\n");
        engineState = EngineState::Terminate;
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        printf("Ctrl-Close event\n");
        engineState = EngineState::Terminate;
        return TRUE;

    default:
        return FALSE;
    }
}

ServerEngine::ServerEngine(int port_num, int tick_ms) : m_port(port_num), m_tick(tick_ms)
{
    m_server = std::make_unique<ServerManager>(port_num, tick_ms);
}

ServerEngine::~ServerEngine()
{
}

void ServerEngine::Run()
{
    /* Setup Control Handler for console exit signals */
    if (!SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        odslog("ERROR: Could not set control handler\n");
        engineState = EngineState::Terminate;
    }

    while (engineState != EngineState::Terminate)
    {
        switch (engineState)
        {
            case EngineState::Startup:
            {
                /* Initialize worlds */
                m_server->Init();

                engineState = EngineState::Active;
                break;
            }

            case EngineState::Active:
            {
                /* Read user input */
                m_server->Process();
                break;
            }

            case EngineState::OneShot:
                m_server->Process();
                engineState = EngineState::Terminate;
                break;

        }
    }
}

void ServerEngine::Shutdown()
{
}
