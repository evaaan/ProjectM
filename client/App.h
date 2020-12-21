#pragma once
#include <comdef.h>
#include <memory>

// forward declarations
class GameEngine;
class Timer;

namespace ComError
{
    inline void throwComError(HRESULT hr, LPCWSTR lpOutputString)
    {
        OutputDebugString(lpOutputString);
        throw _com_error(hr);
    }
}

class App
{
public:
	App(HINSTANCE hInstance, HWND hWindow);
	~App();
    void Initialize();
	void Run();
	std::unique_ptr<GameEngine> m_engine;
    std::shared_ptr<Timer> m_timer;

    const HINSTANCE appInstance;			// handle to an instance of the application
    const HWND appWindow;				    // handle to the window

private:

};
