#include <wincodec.h>  // Windows Imaging Component
#include <wtypes.h>
#include <cmath>
#include <stdexcept>

#include "Direct2D.h"
#include "Animation.h"
#include "App.h"

using namespace ComError;

Sprite::Sprite() : d2d(nullptr), m_bitmap(nullptr), m_x(0.0f), m_y(0.0f), m_layer(Layers::Background), m_drawOrder(0),
        m_opacity(1.0f), m_interpol(D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR)
{}
   
Sprite::Sprite(std::shared_ptr<Direct2D> direct2D, LPCWSTR imageFile, float x, float y,
    Layers layer, unsigned int drawOrder, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol) :
    d2d(direct2D), m_x(x), m_y(y), m_layer(layer), m_drawOrder(drawOrder), m_opacity(opacity), m_interpol(interpol)
{
    HRESULT hr;

    // create decoder
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> bitmapDecoder;
    hr = d2d->WICFactory->CreateDecoderFromFilename(imageFile, NULL,
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
    hr = d2d->WICFactory->CreateFormatConverter(image.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap format converter!");

    // initialize WIC image
    hr = image->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, 
        WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        throwComError(hr, L"Failed to initialize WIC image!");

    // create the bitmap
    hr = d2d->deviceContext->CreateBitmapFromWicBitmap(image.Get(), this->m_bitmap.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to create the bitmap!");
}

void Sprite::draw()
{
    // Size in device-independent pixels
    D2D1_SIZE_F size = m_bitmap->GetSize();
    D2D1_RECT_F dest = { m_x, m_y, m_x + size.width, m_y + size.height };
    D2D1_RECT_F src = { 0, 0, size.width, size.height};

    d2d->deviceContext->DrawBitmap(m_bitmap.Get(), dest, m_opacity, m_interpol, src);
}

void Sprite::update(double deltaTime)
{
    /* Sprite has nothing to update */
}


void Sprite::rescaleDraw(D2D1_RECT_F* destRect, D2D1_RECT_F* sourceRect, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol)
{
    d2d->deviceContext->DrawBitmap(m_bitmap.Get(), destRect, opacity, interpol, sourceRect);
}
    
Sprite::~Sprite()
{
    if (m_bitmap)
        m_bitmap.ReleaseAndGetAddressOf();
}

AnimatedSprite::AnimatedSprite(std::shared_ptr<Direct2D> direct2D, std::shared_ptr<SpriteSheet> spriteSheet, unsigned int activeAnimation,
    float animationFPS, float x, float y, Layers layer, unsigned int drawOrder, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol) :
    spriteSheet(spriteSheet), activeAnimation(activeAnimation), animationFPS(animationFPS), activeAnimationFrame(0)
{
    m_layer = layer;
    m_drawOrder = drawOrder;
    m_x = x;
    m_y = y;
    d2d = direct2D;
    m_bitmap = spriteSheet->spriteSheetBitmap;
    m_opacity = opacity;
    m_interpol = interpol;
}

/* Draw an animated sprite */
void AnimatedSprite::draw()
{
    unsigned int cycle = this->activeAnimation;
    unsigned int frame = this->activeAnimationFrame;
    AnimationCycleData cycleData = spriteSheet->cyclesData[cycle];

    // compute the render target location
    D2D1_RECT_F destRect = { this->m_x - (cycleData.destWidth * cycleData.rotationCenterX),
                            this->m_y - (cycleData.destHeight * cycleData.rotationCenterY),
                            this->m_x + (cycleData.destWidth * (1.0f - cycleData.rotationCenterX)),
                            this->m_y + (cycleData.destHeight * (1.0f - cycleData.rotationCenterY)) };


    // get next frame in the sprite sheet
    float startX = frame * (cycleData.width + cycleData.paddingWidth) + cycleData.borderPaddingWidth;
    float startY = 0;
    for (unsigned int i = 0; i < cycle; i++)
        startY += (spriteSheet->cyclesData[i].height + spriteSheet->cyclesData[i].paddingHeight);
    startY += spriteSheet->cyclesData[0].borderPaddingHeight;
    D2D1_RECT_F sourceRect = { startX, startY, startX + cycleData.width, startY + cycleData.height };

    d2d->deviceContext->DrawBitmap(m_bitmap.Get(), destRect, m_opacity, m_interpol, sourceRect);

}

void AnimatedSprite::update(double deltaTime)
{
    // update how long the currently active frame has been displayed
    frameTime += deltaTime;

    // check whether it is time to change to another frame
    if (frameTime > (1.0f / (double)animationFPS))
    {
        // the number of frames to increment is the integral result of frameTime / (1 / animationFPS), thus frameTime * animationFPS
        activeAnimationFrame += (unsigned int)(frameTime * animationFPS);

        // use modulo computation to make sure to not jump past the last frame
        if (activeAnimationFrame >= spriteSheet->cyclesData[activeAnimation].numberOfFrames)
            activeAnimationFrame = activeAnimationFrame % spriteSheet->cyclesData[activeAnimation].numberOfFrames;
    }

    // set the frame time
    frameTime = std::fmod(frameTime, 1.0f / (double)animationFPS);
}

void AnimatedSprite::changeAnimation(unsigned int cycleToActivate)
{
    if (cycleToActivate > spriteSheet->cyclesData.size())
    {
        // Out of index: Activate first animation cycle
        activeAnimation = 0;
        activeAnimationFrame = 0;
        frameTime = 0.0f;
    }
    else
    {
        activeAnimation = cycleToActivate;
        activeAnimationFrame = 0;
        frameTime = 0.0f;
    }
}

AnimatedSprite::~AnimatedSprite()
{
    spriteSheet = nullptr;
}


SpriteMap::SpriteMap()
{
}

void SpriteMap::addSprite(Sprite* sprite)
{
    Layers layer = sprite->m_layer;
    switch (layer)
    {
    case Layers::Background:
        backgroundMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(sprite->m_drawOrder), std::move(sprite)));
        break;

    case Layers::Characters:
        characterMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(sprite->m_drawOrder), std::move(sprite)));
        break;

    case Layers::UserInterface:
        userInterfaceMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(sprite->m_drawOrder), std::move(sprite)));
        break;
    }
}

void SpriteMap::addSprite(std::shared_ptr<Direct2D> d2d, LPCWSTR imageFile, float x, float y, Layers layer, unsigned int drawOrder)
{
    Layers l = layer;

    switch (l)
    {
    case Layers::Background:
        backgroundMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(drawOrder), new Sprite(d2d, imageFile, x, y, layer, drawOrder)));
        break;

    case Layers::Characters:
        characterMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(drawOrder), new Sprite(d2d, imageFile, x, y, layer, drawOrder)));
        break;

    case Layers::UserInterface:
        userInterfaceMap.insert(std::make_pair<unsigned int, Sprite*>(std::move(drawOrder), new Sprite(d2d, imageFile, x, y, layer, drawOrder)));
        break;
    }
}

void SpriteMap::draw(D2D1_RECT_F* destRect, D2D1_RECT_F* sourceRect, DrawCommands drawCommand, 
    float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol)
{
    D2D1_RECT_F* rect;
    if (destRect)
        rect = destRect;
    else
        rect = NULL;

    switch (drawCommand)
    {
    case DrawCommands::All:
        // draw the background first
        for (auto sprite : backgroundMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);

        // draw the characters
        for (auto sprite : characterMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);

        // draw the user interface
        for (auto sprite : userInterfaceMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);

        break;

    case DrawCommands::onlyBackground:
        // draw the background first
        for (auto sprite : backgroundMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);
        break;

    case DrawCommands::onlyCharacters:
        // draw the background first
        for (auto sprite : characterMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);
        break;

    case DrawCommands::onlyUserInterface:
        // draw the background first
        for (auto sprite : userInterfaceMap)
            sprite.second->rescaleDraw(rect, sourceRect, opacity, interpol);
        break;
    }
}

SpriteMap::~SpriteMap()
{
    for (auto sprite : backgroundMap)
        delete sprite.second;
    for (auto sprite : characterMap)
        delete sprite.second;
    for (auto sprite : userInterfaceMap)
        delete sprite.second;
}

/* Load spriteSheet bitmap*/
SpriteSheet::SpriteSheet(std::shared_ptr<Direct2D> d2d, LPCWSTR spriteSheetFile, std::vector<AnimationCycleData> cyclesData) :
    cyclesData(cyclesData)
{
    HRESULT hr;

    // create decoder
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> bitmapDecoder;
    hr = d2d->WICFactory->CreateDecoderFromFilename(spriteSheetFile, NULL,
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
    hr = d2d->WICFactory->CreateFormatConverter(image.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to get bitmap format converter!");


    // initialize WIC image
    hr = image->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr))
        throwComError(hr, L"Failed to initialize WIC image!");

    // create the bitmap
    hr = d2d->deviceContext->CreateBitmapFromWicBitmap(image.Get(), this->spriteSheetBitmap.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        throwComError(hr, L"Failed to create the bitmap!");

}

SpriteSheet::~SpriteSheet()
{
    if (spriteSheetBitmap)
        spriteSheetBitmap.ReleaseAndGetAddressOf();
}

Background::Background()
{
}

Background::Background(std::shared_ptr<Direct2D> direct2D, LPCWSTR imageFile, float x, float y,
    Layers layer, unsigned int drawOrder, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol) :
    Sprite(direct2D, imageFile, x, y, layer, drawOrder, opacity, interpol) 
{

}

void Background::draw()
{

    // Size in device-independent pixels
    D2D1_SIZE_F size = m_bitmap->GetSize();
    D2D1_RECT_F dest = { m_x, m_y, m_x + size.width, m_y + size.height };
    D2D1_RECT_F src = { 0, 0, size.width, size.height };

    d2d->deviceContext->DrawBitmap(m_bitmap.Get(), dest, m_opacity, m_interpol, src);
}

Background::~Background()
{
}
