#include "GameEngine.h"
#include "Timer.h"
#include "App.h"

App::App(HINSTANCE hInstance, HWND hWindow):
    appInstance(hInstance),
    appWindow(hWindow)
{
}

void App::Initialize()
{
    int tick_ms = 20;  // 50 Hz max

    // Initialize GameEngine
    m_engine = std::make_unique<GameEngine>(appInstance, appWindow, tick_ms);
}

void App::Run()
{
    // Run Engine
    m_engine->Run();
}

App::~App()
{
}
