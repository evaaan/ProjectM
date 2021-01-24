#include <Windows.h>
#include "App.h"
#include "Timer.h"
#include "KeyMap.h"
#include "InputManager.h"
#include "framework.h"
#include "resource.h"
#include "System.h"
#include "World.h"

InputManager::InputManager(std::shared_ptr<Timer> timer, bool &active) :
    m_timer(timer),
    isMinimized(false),
    isMaximized(false),
    isResizing(false),
    msg({ 0 }),
    isActive(active),
    keymap()
{
	// Constructor
}

/* Dispatch the message queue, return False on WM_QUIT */
bool InputManager::ProcessInput()
{
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return true;
}

/* Process windows messages */
LRESULT CALLBACK InputManager::msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_ACTIVATE:
    {
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            isActive = false;
        }
        else
        {
            isActive = true;
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_MENUCHAR:
        // very important for your mental health: disables the crazy beeping sound when pressing a mnemonic key
        return MAKELRESULT(0, MNC_CLOSE);	// simply tell Windows that we want the menu closed

    case WM_KEYDOWN:
        onKeyDown(wParam, lParam);
        break;
    /*
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            // when the window is minimized, set the appropriate window flags and pause the application
            isMinimized = true;
            isMaximized = false;
            p_app->m_engine->isPaused = true;
        }
        else if (wParam == SIZE_MAXIMIZED)
        {
            // when the window is maximized, set the appropriate window flags, resize the graphics and unpause the application
            isMinimized = false;
            isMaximized = true;
            if (p_app->m_engine->hasStarted)
                p_app->m_direct3D->onResize();
            p_app->m_engine->isPaused = false;
        }
        else if (wParam == SIZE_RESTORED)
        {
            if (isMinimized)
            {
                // the window was restored and was previously minimized, thus we set minimized to false, resize the graphics and restart the application
                isMinimized = false;
                if (p_app->m_engine->hasStarted)
                    p_app->m_direct3D->onResize();
                p_app->m_engine->isPaused = false;
            }
            else if (isMaximized)
            {
                // the window was resized and was previously maxmized, thus we set maximized to false, resize the graphics and unpause the game
                isMaximized = false;
                if (p_app->m_engine->hasStarted)
                    p_app->m_direct3D->onResize();
                p_app->m_engine->isPaused = false;
            }
            else if (isResizing)
            {
                // do nothing until the dragging / resizing has stopped (dragging continously sents WM_SIZE messages, it would be extremely slow (and very pointless) to respond to all them)

            }
            else // resize graphics
                if (p_app->m_engine->hasStarted)
                    p_app->m_direct3D->onResize();
        }
        return 0;

    case WM_ENTERSIZEMOVE:
        // the game window is being dragged around, set the isResizing flag and pause the game
        isResizing = true;
        p_app->m_engine->isPaused = true;
        p_app->m_timer->stop();
        return 0;
    
    case WM_EXITSIZEMOVE:
        // the game window is no longer being dragged around, set the isResizing flag to false, resize the graphics and unpause the game
        isResizing = false;
        if (p_app->m_engine->hasStarted)
             p_app->m_direct3D->onResize();
        p_app->m_engine->isPaused = false;
        p_app->m_timer->start();
        return 0;
    */
    case WM_GETMINMAXINFO:
        // prevent the window from becoming too small
        ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/* Push to queue for later retrieval by Input System */
void InputManager::onKeyDown(WPARAM wParam, LPARAM lParam)
{
    m_msgQueue.push_back({ wParam, lParam });
}

InputManager::~InputManager()
{
	// Destructor
}

