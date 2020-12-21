#pragma once
#include <deque>
#include <memory>

#include "KeyMap.h"

// Forward declarations
class App;
class Timer;

struct winMsg
{
    WPARAM wparam;
    LPARAM lparam;
};

class InputManager
{
public:
	InputManager(std::shared_ptr<Timer> timer, bool &paused);
	~InputManager();
	bool ProcessInput();
    virtual LRESULT CALLBACK msgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    INT_PTR About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    std::deque<winMsg> m_msgQueue;
    KeyMap keymap;

private:
    void onKeyDown(WPARAM wParam, LPARAM lParam);
    std::shared_ptr<Timer> m_timer;
    App* app;
    MSG msg;
    bool isResizing;
    bool isMinimized;
    bool isMaximized;
    bool &isPaused;

};
