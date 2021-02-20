#pragma once
#include <memory>
#include <vector>
#include <wrl/client.h> // Windows and COM
#include <wincodec.h>  // Windows Imaging Component
#include <wtypes.h>

#include "Component.h"
#include "ComponentHandle.h"
#include "Entity.h"

// Forward declaration
class SpriteSheet;
class AnimatedSprite;
class Timer;
class Direct3D;
class Direct2D;
class World;
struct AnimationCycleData;

class GraphicManager
{
private:
    std::shared_ptr<Timer> m_timer;
    const double fps_buf_period;  // FPS calculation window in seconds

    std::vector<std::shared_ptr<Entity>> m_entities;

    World* m_currWorld;

public:
    bool showFPS;
    double fps;
    double mspf;
    HWND appWindow;

    std::shared_ptr<Direct3D> m_direct3D;  // m_direct2D has shared ownership of m_direct3D
    std::unique_ptr<Direct2D> m_direct2D;

	GraphicManager(HWND hWindow, std::shared_ptr<Timer>);
	~GraphicManager();
	void Render(double update_time);
	void Present();
    void loadAnimation(LPCWSTR spriteSheetFile, Animation &animation, int frameWidth, int frameHeight, int numFrames);
    void registerWorld(World* world);

    void calculateFrameStatistics(double process_time);
    std::shared_ptr<AnimatedSprite> createWolf(unsigned int x, unsigned int y);
    void onResize();
    void makeTonsOfWolves(unsigned int numberOfWolves);
    void addBackground();
    struct windowSize { long width; long height; };
    windowSize getWindowSize();

    void drawBitmap(ID2D1Bitmap1* bitmap, const D2D1_RECT_F& destRect, float opacity,
        D2D1_BITMAP_INTERPOLATION_MODE interpMode, const D2D1_RECT_F& sourceRect, bool flip);

private:
    std::vector<ID2D1Bitmap1*> bitmaps;
};

