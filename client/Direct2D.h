#pragma once
#include <map>
#include <memory>
#include <wrl/client.h>  // Windows and COM
#include <wincodec.h>	// Windows Imaging Component
#include <WTypes.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "Windowscodecs.lib")


enum class Color { yellow, red, green, blue, black, white };

// Forward declarations
class Direct3D;

class Direct2D
{
public:
    Direct2D(std::shared_ptr<Direct3D> d3d);
    ~Direct2D();

    void printFPS();
    void const createTransformationMatrices();
    Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> getBrush(Color color);

    Microsoft::WRL::ComPtr<ID2D1Device6> device;				    // pointer to the Direct2D device
    Microsoft::WRL::ComPtr<IDWriteFactory6> writeFactory;	        // pointer to the DirectWrite factory
    Microsoft::WRL::ComPtr<IWICImagingFactory2> WICFactory;	        // Windows Imaging Component factory
    Microsoft::WRL::ComPtr<ID2D1Factory7> factory;			        // pointer to the Direct2D factory
    Microsoft::WRL::ComPtr<ID2D1DeviceContext6> deviceContext;      // pointer to the device context
    Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;                 // pointer to the dxgi device

    // text formats
    Microsoft::WRL::ComPtr<IDWriteTextFormat3> textFormatFPS;
    // text layouts
    Microsoft::WRL::ComPtr<IDWriteTextLayout4> textLayoutFPS;

    // standard transformations
    D2D1::Matrix3x2F matrixTranslation;						// translation matrix
    D2D1::Matrix3x2F matrixRotation;						// rotation matrix
    D2D1::Matrix3x2F matrixScaling;							// scaling matrix
    D2D1::Matrix3x2F matrixShearing;						// shearing matrix


private:
    // create devices and resoures
    void createDevice();					// creates the device and its context
    void createBitmapRenderTarget();		// creates the bitmap render target, set to be the same as the backbuffer already in use for Direct3D
    void createHwndRenderTarget();		// Create the window handle render target
    void initializeBrushes();
    void initializeTextFormats();			// initializes the different formats, for now, only a format to print FPS information will be created
    // void createDeviceIndependentResources();// creates device independent resources
    // void createDeviceDependentResources();	// creates device dependent resources
    // void createBrushes();					// initializes different brushes
    // void createStrokeStyles();				// initializes different stroke styles
    // void createGeometries();				// creates geometrical objects
    // void const createTransformationMatrices();	// creates standard transformation matrices

    std::shared_ptr<Direct3D> m_direct3D;
    std::map<Color, Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>> brushes;


};
