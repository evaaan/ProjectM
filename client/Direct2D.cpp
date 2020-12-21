#include <comdef.h>
#include "Direct3D.h"
#include "Direct2D.h"
#include "App.h"

using namespace ComError;

Direct2D::Direct2D(std::shared_ptr<Direct3D> d3d) :
    m_direct3D(d3d)
{
    HRESULT hr = CoInitialize(NULL);  // initialize COM
    if (FAILED(hr))
        throwComError(hr, L"Failed to intiialize COM!");


    createDevice();  // create the device and context

    createBitmapRenderTarget();

    initializeBrushes();

    initializeTextFormats();

    createTransformationMatrices();

}

void Direct2D::createDevice()
{
    HRESULT hr;

    // create the DirectWrite factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &writeFactory);
    if (FAILED(hr))
        throwComError(hr, L"Unable to create the DirectWrite factory!");

    // create WIC factory
    hr = CoCreateInstance(CLSID_WICImagingFactory2, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory2, &WICFactory);
    if (FAILED(hr))
        throwComError(hr, L"Unable to create WIC Factory!");

    // create the Direct2D factory
    D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory2), &options, &factory);
    if (FAILED(hr))
        throwComError(hr, L"Unable to create the Direct2D factory!");

    // Get the dxgi device
    hr = m_direct3D->device.Get()->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice);
    if (FAILED(hr))
        throwComError(hr, L"Unable to get the dxgi device!");

    // create the Direct2D device from the Direct2D factory
    hr = factory->CreateDevice(dxgiDevice.Get(), &device);
    if (FAILED(hr))
        throwComError(hr, L"Unable to create the Direct2D device!");

    // create its context
    hr = device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &deviceContext);
    if (FAILED(hr))
        throwComError(hr, L"Unable to create the Direct2D device context!");
}

void Direct2D::createBitmapRenderTarget()
{
    // specify the desired bitmap properties
    D2D1_BITMAP_PROPERTIES1 bp;
    bp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    bp.dpiX = 96.0f;
    bp.dpiY = 96.0f;
    bp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bp.colorContext = nullptr;

    HRESULT hr;
    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBuffer;
    hr = m_direct3D->swapChain->GetBuffer(0, __uuidof(IDXGISurface), &dxgiBuffer);
    if (FAILED(hr))
        throwComError(hr, L"Unable to retrieve the back buffer!");

    // create the bitmap
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;
    hr = deviceContext->CreateBitmapFromDxgiSurface(dxgiBuffer.Get(), &bp, &targetBitmap);
    if (FAILED(hr))
        throwComError(hr, L" Unable to create the Direct2D bitmap from the DXGI surface!");

    // set the newly created bitmap as render target
    deviceContext->SetTarget(targetBitmap.Get());

     // set antialiasing for text to grayscale
    deviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

//void Direct2D::createHwndRenderTarget()
//{
//    HRESULT hr;
//    RECT rc;
//    GetClientRect(m_direct3D->appWindow, &rc);
//
//    D2D1_SIZE_U size = D2D1::SizeU(
//        rc.right - rc.left,
//        rc.bottom - rc.top
//    );
//
//    // Create a Direct2D render target.
//    hr = factory->CreateHwndRenderTarget(
//        D2D1::RenderTargetProperties(),
//        D2D1::HwndRenderTargetProperties(m_direct3D->appWindow, size),
//        &hwndRenderTarget
//    );
//}

void Direct2D::initializeBrushes()
{
    brushes.insert({ Color::yellow, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    brushes.insert({ Color::red, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    brushes.insert({ Color::green, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    brushes.insert({ Color::blue, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    brushes.insert({ Color::black, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    brushes.insert({ Color::white, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>() });
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &brushes.at(Color::yellow));
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Firebrick), &brushes.at(Color::red));
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &brushes.at(Color::green));
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DodgerBlue), &brushes.at(Color::blue));
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brushes.at(Color::black));
    deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brushes.at(Color::white));
}

/* Get a brush with the specified color */
Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Direct2D::getBrush(Color color)
{
    return brushes.at(color);
}

void Direct2D::initializeTextFormats()
{
    // FPS text format
    writeFactory.Get()->CreateTextFormat(
        L"Lucida Console",                      // Font name
        nullptr,                                // Font collection
        DWRITE_FONT_WEIGHT_LIGHT,               // Weight
        DWRITE_FONT_STYLE_NORMAL,               // Style
        DWRITE_FONT_STRETCH_NORMAL,             // Stretch
        12.0f,                                  // Size
        L"en-GB",                               // Locale name
        (IDWriteTextFormat **)textFormatFPS.GetAddressOf()                          // Text format
    );
    textFormatFPS->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    textFormatFPS->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void Direct2D::printFPS()
{
    if (textLayoutFPS)
        deviceContext->DrawTextLayout(D2D1::Point2F(2.0f, 5.0f), textLayoutFPS.Get(), getBrush(Color::yellow).Get());
}

void const Direct2D::createTransformationMatrices()
{
    matrixTranslation = D2D1::Matrix3x2F::Translation(1, 0);	// standard translation towards the x-axis
    matrixScaling = D2D1::Matrix3x2F::Scale(2, 2);				// standard scaling (doubling)
    matrixRotation = D2D1::Matrix3x2F::Rotation(45);			// 45 degrees clockwise rotation
    matrixShearing = D2D1::Matrix3x2F::Skew(45, 0);				// 45 degrees counterclockwise in x-direction

}

Direct2D::~Direct2D()
{
    WICFactory.ReleaseAndGetAddressOf();
    CoUninitialize();
}
