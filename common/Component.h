#pragma once
#include <bitset>
#include <type_traits>
#include <wrl/client.h>
#include <d2d1_3.h>
#include <array>
#include <map>
#include <set>

#include "KeyMap.h"

/* This is the base Component class. To create your own component:
* struct Position : public Component<Position>
* {
*     int x;
*     int y;
* }
*/

// Forward declarations
class Direct2D;
class SpriteSheet;
enum class Color;

/* familyCounter tracks the ComponentManager the Component is assigned to.*/
struct ComponentCounter {
    static int familyCounter;
};

template <typename ComponentType>
struct Component {

    static inline int family()
    {
        static int family = ComponentCounter::familyCounter++;
        return family;
    }
};

template <typename C>
static int GetComponentFamily()
{
    return Component<typename std::remove_const<C>::type>::family();
}

struct Vector2
{
    double x;
    double y;
};

/* Keyboard and mouse state */
struct InputSingleton : public Component<InputSingleton>
{
    std::bitset<NUM_KEYBINDINGS> keyDownState;
};

/* Debug windows */
struct ImGuiSingleton : public Component<ImGuiSingleton>
{
};

/* Sets of collisions */
struct CollisionSingleton : public Component<CollisionSingleton>
{
    std::map<int, std::set<int>> collisionMap;  // Map from entity to all its collisions
    std::map<int, std::set<int>> solidCollisionMap;  // Map from entity to all its solid collisions
    std::map<int, std::set<int>> ledgeCollisionMap;  // Map from entity to all its ledge collisions
};

/* Position and size on the display */
struct Transform : public Component<Transform>
{
    int x;
    int y;
    int width;
    int height;
};

enum class BodyType { Player, Transparent, Ledge, Solid };

/* Affected by physics */
struct Dynamic : public Component<Dynamic>
{
    double width;           // hitbox width
    double height;
    Vector2 pos;            // Position
    Vector2 prev_pos;       // Position in previous frame
    Vector2 vel;            // Velocity (speed (pix/sec) and direction vector)
    Vector2 accel;          // Acceleration
    BodyType type;       // trasparent, ledge, or solid
};

struct AnimationCycle
{
public:
    int startFrame;		// the index of the first frame of an animation
    int numberOfFrames;	// the total numbers of frames in the animation
    int	width,			// the width of each frame
        height,					// the height of each frame
        destWidth,              // width of each frame in render target
        destHeight,             // height of each frame in render targe
        paddingWidth,			// width of the padding
        paddingHeight,			// height of the padding
        borderPaddingWidth,		// width of the border padding
        borderPaddingHeight;	// height of the border padding

};

enum class Layer { Background, Characters, UserInterface };
enum class DrawCommand { All, onlyBackground, onlyCharacters, onlyUserInterface };

/* Uses a Sprite Sheet */
struct Animation : public Component<Animation>
{
    Layer m_layer;								     // the layer the bitmap belongs to
    unsigned int drawOrder;                          // draw order of the bitmap
    double opacity;
    D2D1_BITMAP_INTERPOLATION_MODE interpol;
    int activeAnimation;                       // Sprite sheet row
    int activeAnimationFrame;                  // Sprite sheet column0
    int numCycles;                             // Number of cycles
    double animationFPS = 24.0;
    double frameTime;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> bitmap;         // bitmap of the sprite sheet
    std::array<AnimationCycle, 10> cyclesData;           // config for animation
};

/* Has an outline (with color and width) */
struct Outline : public Component<Outline>
{
    Color color;
    int width;
};
