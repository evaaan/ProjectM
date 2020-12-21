#pragma once
#include <string.h>
#include <map>
#include <memory>
#include <vector>

#include <wrl/client.h>
#include <d2d1_3.h>

// Forward declarations
class Direct2D;

// layers are used in the painter's algorithm
enum class Layers { Background, Characters, UserInterface };

// specify what to draw
enum class DrawCommands { All, onlyBackground, onlyCharacters, onlyUserInterface };

class Sprite
{
public:
    Layers m_layer;								  // the layer the bitmap belongs to
    unsigned int m_drawOrder;                       // draw order of the bitmap
    float m_x, m_y;                                   // position
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> m_bitmap;  // bitmap of the image
    float m_opacity;
    D2D1_BITMAP_INTERPOLATION_MODE m_interpol;
    std::shared_ptr<Direct2D> d2d;                // pointer to the Direct2D class

    Sprite();
    // loads an image from the disk and saves it as a sprite
    Sprite(std::shared_ptr<Direct2D> d2d, LPCWSTR imageFile, float x = 0.0f, float y = 0.0f,
        Layers layer = Layers::Characters, unsigned int drawOrder = 0, float m_opacity = 1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE m_interpol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    ~Sprite();
        
    // draws the sprite
    // destRect and sourceRect are size and position of the render and bitmap target in device-independent pixels
    virtual void draw();
    virtual void update(double deltaTime);
    void rescaleDraw(D2D1_RECT_F* destRect, D2D1_RECT_F* sourceRect, float opacity, D2D1_BITMAP_INTERPOLATION_MODE interpol);

};



class Background : public Sprite
{
public:
    Background();
    Background(std::shared_ptr<Direct2D> d2d, LPCWSTR imageFile, float x = 0.0f, float y = 0.0f,
        Layers layer = Layers::Characters, unsigned int drawOrder = 0, float m_opacity = 1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE m_interpol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    ~Background();

    void draw();


private:

};

struct AnimationCycleData
{
public:
    LPCWSTR name;					// name of the animation
    unsigned int startFrame;		// the index of the first frame of an animation
    unsigned int numberOfFrames;	// the total numbers of frames in the animation
    float	width,					// the width of each frame
            height,					// the height of each frame
            destWidth,              // width of each frame in render target
            destHeight,             // height of each frame in render targe
            rotationCenterX,		// rotation center x-coordinate
            rotationCenterY,		// rotation center y-coordinate
            paddingWidth,			// width of the padding
            paddingHeight,			// height of the padding
            borderPaddingWidth,		// width of the border padding
            borderPaddingHeight;	// height of the border padding

};

class SpriteSheet
{

public:
    std::vector<AnimationCycleData> cyclesData;              // config for animation
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> spriteSheetBitmap;  // bitmap of the sprite sheet

    // Initialize WIC image and bitmap
    SpriteSheet(std::shared_ptr<Direct2D> d2d, LPCWSTR spriteSheetFile, std::vector<AnimationCycleData> cyclesData);
    ~SpriteSheet();
};

class AnimatedSprite : public Sprite
{
private:
    std::shared_ptr<SpriteSheet> spriteSheet;
    unsigned int activeAnimation;        // Sprite sheet row
    unsigned int activeAnimationFrame;   // Sprite sheet column
    float animationFPS = 24.0f;
    double frameTime;


public:
    AnimatedSprite(std::shared_ptr<Direct2D> d2d, std::shared_ptr<SpriteSheet> m_spriteSheet,
        unsigned int m_activeAnimation = 0, float m_animationFPS = 24, 
        float m_x = 0.0f, float m_y = 0.0f, 
        Layers m_layer = Layers::Characters, unsigned int m_drawOrder = 0,
        float m_opacity = 1.0f, D2D1_BITMAP_INTERPOLATION_MODE m_interpol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    ~AnimatedSprite();

    void draw();
    void update(double deltaTime);
    void changeAnimation(unsigned int cycleToActivate);  // activate the specified animation cycle

};

class SpriteMap
{
private:
    std::multimap<unsigned int, Sprite*> backgroundMap;		// holds the background sprites
    std::multimap<unsigned int, Sprite*> characterMap;		// holds the character sprites
    std::multimap<unsigned int, Sprite*> userInterfaceMap;	// holds the user interface sprites

public:
    // constructors
    SpriteMap();
    ~SpriteMap();

    // populate the sprite map
    void addSprite(Sprite* sprite);							// adds an existing sprite to its correct map
    void addSprite(std::shared_ptr<Direct2D> d2d, LPCWSTR imageFile, float x = 0.0f, float y = 0.0f, Layers layer = Layers::Characters, unsigned int drawOrder = 0);	// create a new sprite and adds it to sprite map

    // draw the sprites
    void draw(D2D1_RECT_F* destRect, D2D1_RECT_F* sourceRect, DrawCommands drawCommand = DrawCommands::All, float opacity = 1.0f, D2D1_BITMAP_INTERPOLATION_MODE interPol = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);	// draw sprites based on layers and draw order
};