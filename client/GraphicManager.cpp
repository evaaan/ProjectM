#pragma comment(lib, "d2d1")
#pragma comment(lib, "yaml-cppd.lib")
#include <comdef.h>
#include <d2d1.h>
#include <sstream>
#include <vector>
#include <numeric>
#include <random>
#include <yaml-cpp/yaml.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

#include "Component.h"
#include "Animation.h"
#include "Entity.h"
#include "Timer.h"
#include "Direct2D.h"
#include "Direct3D.h"
#include "World.h"
#include "GraphicManager.h"
#include "App.h"
#include "Utilities.h"
using namespace ComError;

GraphicManager::GraphicManager(HWND hWindow, std::shared_ptr<Timer> timer) :
    appWindow(hWindow),
    m_timer(timer),
    m_currWorld(nullptr),
    fps(0.0),
    mspf(0.0),
    showFPS(true),
    fps_buf_period(0.05)  // how often to update FPS (seconds)
{
    m_direct3D = std::make_shared<Direct3D>(appWindow, m_timer);       // Direct3D
    onResize();
    m_direct2D = std::make_unique<Direct2D>(m_direct3D);               // Direct2D

    // addBackground();
}

GraphicManager::~GraphicManager()
{
    /* Release all bitmaps. Eventually, we should dynamically release bitmaps as they are no longer needed. */
    for (auto bitmap : bitmaps)
        if (!(bitmap == NULL))
            bitmap->Release();
}

GraphicManager::windowSize GraphicManager::getWindowSize()
{
    windowSize window{ 0, 0 };
    RECT windowDimensions;
    if (GetWindowRect(appWindow, &windowDimensions))
        window.width = windowDimensions.right - windowDimensions.left;
        window.height = windowDimensions.bottom - windowDimensions.top;
    return window;
}

void GraphicManager::addBackground()
{
    std::vector<AnimationCycleData> bgAnimationsCycles;
    AnimationCycleData cycle;

    cycle.startFrame = 0;
    cycle.numberOfFrames = 1;
    cycle.width = 384;
    cycle.height = 224;
    cycle.destWidth = cycle.width;
    cycle.destHeight = cycle.height;
    cycle.paddingWidth = 0;
    cycle.paddingHeight = 0;
    cycle.borderPaddingWidth = 0;
    cycle.borderPaddingHeight = 0;
    bgAnimationsCycles.push_back(cycle);
}

/* convert string to wstring. Valid for single-byte encodings only (ASCII, etc). */
void StringToWString(std::wstring& ws, const std::string& s)
{
    std::wstring wsTmp(s.begin(), s.end());
    ws = wsTmp;
}

void GraphicManager::loadAnimationFromYaml(Animation& animation, std::string name, std::string config_file)
{
    HRESULT hr;
    AnimationCycle cycle;

    /* PARSE YAML */
    YAML::Node config = YAML::LoadFile(config_file);
    YAML::Node asset = config[name];
    std::string path_to_asset = "../../" + asset["path"].as<std::string>();
    std::wstring wpath_to_asset;
    StringToWString(wpath_to_asset, path_to_asset);

    /* CREATE SPRITE SHEET BITMAP */
    // create decoder
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> bitmapDecoder;
    hr = m_direct2D->WICFactory->CreateDecoderFromFilename(wpath_to_asset.c_str(), NULL,
        GENERIC_READ, WICDecodeMetadataCacheOnLoad, bitmapDecoder.GetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to load BMP!");

    // get correct frame
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = bitmapDecoder->GetFrame(0, frame.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap frame!");

    // create format converter
    Microsoft::WRL::ComPtr<IWICFormatConverter> image;
    hr = m_direct2D->WICFactory->CreateFormatConverter(image.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap format converter!");


    // initialize WIC image
    hr = image->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        throwComError(hr, L"Failed to initialize WIC image!");

    // create the bitmap and store in animation
    hr = m_direct2D->deviceContext->CreateBitmapFromWicBitmap(image.Get(), animation.bitmap.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to create the bitmap!");

    /* BUILD COMPONENT */
    animation.m_layer = Layer::Characters;
    animation.drawOrder = 0;
    animation.opacity = 1.0;
    animation.interpol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
    animation.activeAnimation = 0;
    animation.activeAnimationFrame = 0;
    animation.frameTime = 0.0;
    animation.animationFPS =    asset["fps"].as<int>();

    // man cycle
    cycle.startFrame =          asset["startFrame"].as<int>();
    cycle.width =               asset["width"].as<int>();
    cycle.height =              asset["height"].as<int>();
    cycle.destWidth =           cycle.width;
    cycle.destHeight =          cycle.height;
    cycle.paddingWidth =        asset["paddingWidth"].as<int>();
    cycle.paddingHeight =       asset["paddingHeight"].as<int>();
    cycle.borderPaddingWidth =  asset["borderPaddingWidth"].as<int>();
    cycle.borderPaddingHeight = asset["borderPaddingHeight"].as<int>();
    cycle.numberOfFrames =      asset["numberOfFrames"].as<int>();
    animation.cyclesData[0] =   cycle;

    /* Store bitmap pointer to release at GraphicManager destructor */
    ID2D1Bitmap1* bitmapPtr = animation.bitmap.Get();
    bitmaps.push_back(bitmapPtr);
}

/* Create an Animation Component */
void GraphicManager::loadAnimation(LPCWSTR spriteSheetFile, Animation &animation, int frameWidth, int frameHeight, int numFrames)
{
    HRESULT hr;
    AnimationCycle cycle;

    /* CREATE SPRITE SHEET BITMAP */
    // create decoder
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> bitmapDecoder;
    hr = m_direct2D->WICFactory->CreateDecoderFromFilename(spriteSheetFile, NULL,
        GENERIC_READ, WICDecodeMetadataCacheOnLoad, bitmapDecoder.GetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to load BMP!");

    // get correct frame
    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = bitmapDecoder->GetFrame(0, frame.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap frame!");

    // create format converter
    Microsoft::WRL::ComPtr<IWICFormatConverter> image;
    hr = m_direct2D->WICFactory->CreateFormatConverter(image.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap format converter!");


    // initialize WIC image
    hr = image->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        throwComError(hr, L"Failed to initialize WIC image!");

    // create the bitmap and store in animation
    hr = m_direct2D->deviceContext->CreateBitmapFromWicBitmap(image.Get(), animation.bitmap.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to create the bitmap!");

    /* BUILD COMPONENT */
    animation.m_layer = Layer::Characters;
    animation.drawOrder = 0;
    animation.opacity = 1.0;
    animation.interpol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
    animation.activeAnimation = 0;
    animation.activeAnimationFrame = 0;
    animation.frameTime = 0.0;

    // man cycle
    cycle.startFrame = 0;
    cycle.width = frameWidth;
    cycle.height = frameHeight;
    cycle.destWidth = cycle.width;
    cycle.destHeight = cycle.height;
    cycle.paddingWidth = 0;
    cycle.paddingHeight = 0;
    cycle.borderPaddingWidth = 0;
    cycle.borderPaddingHeight = 0;
    cycle.numberOfFrames = numFrames;
    animation.cyclesData[0] = cycle;

    /* Store bitmap pointer to release at GraphicManager destructor */
    ID2D1Bitmap1* bitmapPtr = animation.bitmap.Get();
    bitmaps.push_back(bitmapPtr);
}

void GraphicManager::Render(double process_time)
{   
    // clear the back buffer and the depth/stencil buffer
    m_direct3D->clearBuffers();

    // Render world
    m_direct2D->deviceContext->BeginDraw();
    //m_direct2D->matrixScaling = D2D1::Matrix3x2F::Scale(3, 3, D2D1::Point2F(575, 385));
    //m_direct2D->deviceContext->SetTransform(m_direct2D->matrixScaling);
    //m_direct2D->deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());

    if (m_currWorld != nullptr)
        m_currWorld->render();
    // else show Loading Screen, etc.

        // print FPS info
    calculateFrameStatistics(process_time * 1000.0);
    if (showFPS)
        m_direct2D->printFPS();

    m_direct2D->deviceContext->EndDraw();



}

void GraphicManager::Present()
{
    m_direct3D->present();
}

/* Wrapper around direct2d DrawBitmap */
void GraphicManager::drawBitmap(ID2D1Bitmap1* bitmap, const D2D1_RECT_F& destRect, float opacity,
    D2D1_BITMAP_INTERPOLATION_MODE interpMode, const D2D1_RECT_F& sourceRect, bool flip)
{
    if (flip)
    {
        D2D1_POINT_2F imageCenter = D2D1::Point2F(
            destRect.left + 64 / 2,
            destRect.top + 64 / 2
        );
        
        // Horizontal translation in 2 steps:
        // 1. Flip bitmap horizontally about y-axis
        // 2. Translate left by twice the x-distance to the bitmap center
        m_direct2D->deviceContext->SetTransform(
            D2D1::Matrix3x2F(-1, 0, 0, 1, 0, 0) * D2D1::Matrix3x2F::Translation(2 * destRect.left + (destRect.right-destRect.left), 0)
            // D2D1::Matrix3x2F::Rotation(-20, imageCenter)
        );
    }
    m_direct2D->deviceContext->DrawBitmap(bitmap, destRect, opacity, interpMode, sourceRect);

    // un-do transform
    m_direct2D->deviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
}

/* Get a reference to the current world so we can render it. */
void GraphicManager::registerWorld(World* world)
{
    m_currWorld = world;
}

/* FPS frame queue
* Initialize std::queue
* if time elapsed > threshold and showFPS
* queue::push fps
* if total_frames > queue size
* queue::pop
* calculate average fps of queue and display
*/
void GraphicManager::calculateFrameStatistics(double process_ms)
{
    HRESULT hr;
    static double elapsedTime;  // time since last call
    double fps;
    
    // Compute fps over running frame
    if (showFPS && (m_timer->getTotalTime() - elapsedTime) >= fps_buf_period)
    {
        fps = 1.0 / m_timer->getDeltaTime();

        // Update textLayoutFPS
        std::wostringstream outFPS;
        std::wostringstream outProcess;
        outFPS.precision(6);
        outFPS << "FPS: " << fps << std::endl;
        outFPS << "Processing: " << process_ms << " ms" << std::endl;
        hr = m_direct2D->writeFactory->CreateTextLayout(
            outFPS.str().c_str(),
            (UINT32)outFPS.str().size(),
            m_direct2D->textFormatFPS.Get(),
            200.0,
            200.0,
            (IDWriteTextLayout**)m_direct2D->textLayoutFPS.GetAddressOf()
        );
        if (FAILED(hr))
            throwComError(hr, L"Failed to write FPS!");


        elapsedTime += fps_buf_period;
    }

}

void GraphicManager::onResize()
{
    HRESULT hr;

    // release and reset all resources
    if (m_direct2D)
        m_direct2D->deviceContext->SetTarget(nullptr);

    m_direct3D->deviceContext->ClearState();
    m_direct3D->renderTargetView = nullptr;
    m_direct3D->depthStencilView = nullptr;

    // resize the swap chain
    hr = m_direct3D->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(hr))
        throwComError(hr, L"Direct3D was unable to resize the swap chain!");

    // (re)create the render target view, and pass it the back buffer
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = m_direct3D->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr))
        throwComError(hr, L"Direct3D was unable to get the back buffer!");

    hr = m_direct3D->device->CreateRenderTargetView(backBuffer.Get(), NULL, &m_direct3D->renderTargetView);
    if (FAILED(hr))
        throwComError(hr, L"Direct3D was unable to create the render target view!");

    // create the depth and stencil buffer
    D3D11_TEXTURE2D_DESC dsd;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> dsBuffer;
    backBuffer->GetDesc(&dsd);
    dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsd.Usage = D3D11_USAGE_DEFAULT;
    dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_direct3D->device->CreateTexture2D(&dsd, NULL, dsBuffer.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Direct3D was unable to create a 2D-texture!");
    hr = m_direct3D->device->CreateDepthStencilView(dsBuffer.Get(), NULL, m_direct3D->depthStencilView.GetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Direct3D was unable to create the depth and stencil buffer!");

    // Activate the depth and stencil buffer
    m_direct3D->deviceContext->OMSetRenderTargets(1, m_direct3D->renderTargetView.GetAddressOf(), m_direct3D->depthStencilView.Get());

    // Set the viewport to the entire back buffer
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = (float)dsd.Width;
    vp.Height = (float)dsd.Height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_direct3D->deviceContext->RSSetViewports(1, &vp);
}
