#include "App.h"
#include "Timer.h"
#include "GameManager.h"
#include "GraphicManager.h"
#include "InputManager.h"
#include "GameEngine.h"

using namespace GameControl;

GameEngine::GameEngine(HINSTANCE hInstance, HWND hWindow, int tick_ms) :
    appInstance(hInstance),
    appWindow(hWindow),
	m_engineState(EngineState::WaitingForResources),
    hasStarted(false),
    isActive(true),
    m_tick(tick_ms)
{

    // Create the timer and start it
    m_timer = std::make_shared<Timer>();
    m_timer->start();
    m_graphic = std::make_unique<GraphicManager>(appWindow, m_timer);     // Graphics
    m_input = std::make_unique<InputManager>(m_timer, isActive);                    // Input
    m_game = std::make_unique<GameManager>(m_timer, m_input.get(), m_graphic.get(), tick_ms);  // Game

}

void GameEngine::Init()
{
}

GameEngine::~GameEngine()
{
}

/* Run the GameEngine state machine until the user exits the program */
void GameEngine::Run()
{
    hasStarted = true;
    double process_time = 0.0;
	while (m_engineState != EngineState::Terminate)
	{
		/* Update user input, handle exit signal */
		if (!m_input->ProcessInput())
		{
			m_engineState = EngineState::Terminate;
			continue;
		}
        switch (m_engineState)
        {
                /* Transition to dynamic after resources load */
            case EngineState::WaitingForResources:
            {
                /* Initialize game world. */
                m_game->Init();
                m_engineState = EngineState::Dynamics;
                break;
            }

            /* Update world */
            case EngineState::Dynamics:
            {
                process_time = m_game->Update();
                break;
            }
        }

        /* Render and present */
        m_graphic->Render(process_time);
        m_graphic->Present();
	}
}

void GameEngine::Shutdown()
{
}
