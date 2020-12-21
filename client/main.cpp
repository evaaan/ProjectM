// main.cpp : Defines the entry point for the application.
#include <memory>
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "framework.h"
#include "Resource.h"
#include "GameEngine.h"
#include "App.h"

#define MAX_LOADSTRING 100

// Global Variables
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


/*The Window class requires a pointer to an application-defined function
* for processing Windows Messages (lpfnWndProc). Since we don't initialize
* the InputManager until after the App is created, pass the Window
* a raw pointer that will be assigned after initialization completes */
App* appPtr = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                RunApp(HINSTANCE, HWND);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    WNDCLASSEXW wcex;
    hInst = hInstance; // Store instance handle in our global variable

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENTVSPROJ, szWindowClass, MAX_LOADSTRING);

    // Initialize Window class
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_CLIENTVSPROJ));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLIENTVSPROJ);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    RegisterClassExW(&wcex);

    // Show Window
    HWND hWnd = CreateWindowW(szWindowClass,szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
                              CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return 1;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    OutputDebugString(L"Window created\n");

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENTVSPROJ));

    // Run app
    RunApp(hInst, hWnd);
    
    return 0;
}

/* Run the app. Store a global reference for WndProc callback.*/
void RunApp(HINSTANCE hInst, HWND hWnd)
{
    /* create and initialize */
    std::unique_ptr<App> app = std::make_unique<App>(hInst, hWnd);
    app->Initialize();

    /* Point WndProc callbacks to app */
    appPtr = app.get();
    app->Run();

    /* Return to default Windows Proc */
    appPtr = nullptr;
}
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/* Process windows messages. Use default processing until app is initialized. */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    if (appPtr != nullptr)
        return appPtr->m_engine->m_input->msgProc(hWnd, message, wParam, lParam);
    else
        return DefWindowProc(hWnd, message, wParam, lParam);
}


