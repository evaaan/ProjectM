#include <comdef.h>

#include "App.h"
#include "Timer.h"
#include "Direct3D.h"

using namespace ComError;

Direct3D::Direct3D(HWND hWindow, std::shared_ptr<Timer> timer) :
    appWindow(hWindow),
    m_timer(timer)
{
	HRESULT hr;
	D3D_FEATURE_LEVEL featureLevel;
    supportedModes = nullptr;
    numberOfSupportedModes = 0;
    desiredColorFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

    // Support surfaces with different color channel ordering than default. Need for Direct2D compatibility.
	unsigned int createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	hr = D3D11CreateDevice(
		NULL,                               // IDXGIAdapter; the GPU interface, set to NULL to use default adapter
		D3D_DRIVER_TYPE_HARDWARE,           // D3D_DRIVER_TYPE; use hardware acceleration
		0,                                  // HMODULE; handle to DLL for software rasterizer. Not used, so set to 0
		createDeviceFlags,                  // UINT; device flags
		NULL,                               // D3D_FEATURE_LEVEL; NULL to get greatest feature level available
		0,                                  // UINT; length of above argument is zero
		D3D11_SDK_VERSION,                  // UINT; Use D3D11 SDK
		&device,                            // ID3D11Device; pointer to the pointer to the DirectX device
		&featureLevel,                      // D3D_FEATURE_LEVEL; If successful, first successful D3D_FEATURE_LEVEL is stored here
		&deviceContext                      // ID3D11DeviceContext; pointer to the pointer to the DirectX device context
	);
    if (FAILED(hr))
        throwComError(hr, L"D3D11CreateDevice Failed!");

    else if (featureLevel < D3D_FEATURE_LEVEL_11_0)
        throwComError(hr, L"DirectX11 feature level not supported!");

    // Query for DXGI device and create swap chain
    createDxgiSwapChain();
}

/* Get a DXGI device from the D3D11 interface and create the swap chain.
* This function assumes that D3D11CreateDevice was successful.
*/
void Direct3D::createDxgiSwapChain()
{
    // Build swap chain description
    HRESULT hr;
    DXGI_SWAP_CHAIN_DESC scd;
    
    scd.BufferDesc.Width = 0;													// Use automatic sizing
    scd.BufferDesc.Height = 0;
    scd.BufferDesc.RefreshRate.Numerator = 0;									// refresh rate: 0 -> do not care
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = desiredColorFormat;						            // This is the most common format							
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		// unspecified scan line ordering
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;						// unspecified scaling
    scd.SampleDesc.Count = 1;													// disable multisampling, no anti aliasing
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// use back buffer as render target
    scd.BufferCount = 3;														// the number of buffers in the swap chain (including the front buffer)
    scd.OutputWindow = appWindow;           					                // set the main window as output target
    scd.Windowed = true;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;								// flip mode and discarded buffer after presentation
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;							// allow mode switching

    // First, retrieve DXGI device
    hr = device.As(&dxgiDevice);
    if (FAILED(hr))
        throwComError(hr, L"DirectX failed to retrieve the underlying DXGI device!");

    // now identify the physical GPU this device is running on
    hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"The DXGI Device was unable to get the GPU adapter!");

    // Retrieve the dxgi factory
    hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), &dxgiFactory);
    if (FAILED(hr))
        throwComError(hr, L"Failed to retrieve the DXGI Factory!");

    // Create the swap chain from the factory
    hr = dxgiFactory->CreateSwapChain(device.Get(), &scd, swapChain.GetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to create Swap Chain!");

    // Enumerate all display modes and resolutions
    IDXGIOutput *output = nullptr;
    hr = swapChain->GetContainingOutput(&output);
    if (FAILED(hr))
        throwComError(hr, L"Unable to retrieve output adapter!");


    OutputDebugString(L"GetDisplayModeList\n");

    // Get number of supported display modes for desired format1
    hr = output->GetDisplayModeList(desiredColorFormat, 0, &numberOfSupportedModes, NULL);
    if (FAILED(hr))
        throwComError(hr, L"Unable to list all supported display modes!");

    OutputDebugString(L"create supportedModes\n");

    // Get list of supported resolutions
    supportedModes = new DXGI_MODE_DESC[numberOfSupportedModes];
    ZeroMemory(supportedModes, sizeof(DXGI_MODE_DESC) * numberOfSupportedModes);

    OutputDebugString(L"Fill display mode list\n");

    // Fill the array with the available display modes
    hr = output->GetDisplayModeList(desiredColorFormat, 0, &numberOfSupportedModes, supportedModes);
    if (FAILED(hr))
        throwComError(hr, L"Unable to get list of display modes!");

    // Release the output adapter
    output->Release();

}

/* Call swapchain->present to load next buffer */
HRESULT Direct3D::present()
{
    HRESULT hr = swapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
    if (FAILED(hr) && hr != DXGI_ERROR_WAS_STILL_DRAWING)
    {
        throwComError(hr, L"Direct3D failed to present the scene!");
    }
    deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
    return hr;
}

void Direct3D::clearBuffers()
{

    // clear the back buffer and depth / stencil buffer
    float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView.Get(), black);
    deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

Direct3D::~Direct3D()
{
    delete[] supportedModes;

}
