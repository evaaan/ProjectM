#pragma once
#include <memory>
#include <wrl/client.h>  // Windows and COM
#include <d3d11_4.h>
#pragma comment (lib, "d3d11.lib")

// forward declarations
class App;
class Timer;

class Direct3D
{
private:
    unsigned int numberOfSupportedModes;					// the number of supported screen modes for the desired colour format
    DXGI_MODE_DESC* supportedModes;							// list of all supported screen modes for the desired colour format
    DXGI_FORMAT desiredColorFormat;

    void createDxgiSwapChain();

    std::shared_ptr<Timer> m_timer;



public:
	Direct3D(HWND hWindow, std::shared_ptr<Timer> timer);
	~Direct3D();
    void onResize();
    HRESULT present();
    void clearBuffers();                                    // Clear render view and stencil view

    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;	// the rendering target
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;	// the depth and stencil buffer

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;

    HWND appWindow;

};

