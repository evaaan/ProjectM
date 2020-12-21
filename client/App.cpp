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
    // Create the timer and start it
    m_timer = std::make_shared<Timer>();
    m_timer->start();

    // Initialize GameEngine
    m_engine = std::make_unique<GameEngine>(appInstance, appWindow, m_timer);
}

void App::Run()
{
    // Run Engine
    m_engine->Run();
}

App::~App()
{
}
