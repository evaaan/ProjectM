#pragma once
#include <memory>
#include <Windows.h>

#include "InputManager.h"

// Forward declarations
class App;
class Timer;
class GameManager;
class GraphicManager;

namespace GameControl
{
	enum class EngineState
	{
		Deactivated,
		WaitingForResources,
		Dynamics,
		Terminate,
	};
};

class GameEngine
{
public:
	GameEngine(HINSTANCE hInstance, HWND hWindow, std::shared_ptr<Timer>);
	~GameEngine();

    void Init();
	void Run();
	void Shutdown();

    // game state
    bool isPaused;
    bool hasStarted;

    /* Accessible by main window */
    std::unique_ptr<InputManager> m_input;              // User input functions

private:
	GameControl::EngineState m_engineState;
    std::shared_ptr<Timer> m_timer;
    std::unique_ptr<GameManager> m_game;                // ECS System
    std::unique_ptr<GraphicManager> m_graphic;          // Graphics and rendering

    HINSTANCE appInstance;
    HWND appWindow;


};

