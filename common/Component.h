#pragma once
#include <bitset>
#include <type_traits>
#include <wrl/client.h>
#include <d2d1_3.h>
#include <array>
#include <map>
#include <memory>
#include <set>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <GameNetworkingSockets/steam/isteamnetworkingutils.h>
#include <Windows.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <iostream>
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
struct Entity;
struct ComponentMask;
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

/* Keyboard state. See enum Action in KeyMap.cpp */
struct KeyState : public Component<KeyState>
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
        height,					// the height of each frame+
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

/* Server Steam Socket */
struct ServerSocketSingleton : public Component<ServerSocketSingleton>
{
    int listen_port = 35656;
    ISteamNetworkingSockets* m_pInterface;  // SteamNetworkingSockets() returns raw pointer
    std::shared_ptr<SteamNetworkingIPAddr> serverLocalAddr;  // Use shared_ptr for copy constructor
    std::shared_ptr<SteamNetworkingConfigValue_t> opt;  // Use shared_ptr for copy constructor
    HSteamListenSocket m_hListenSock;  // uint32 handle
    HSteamNetPollGroup m_hPollGroup;  // uint32 handle
    std::map<HSteamNetConnection, boost::uuids::uuid> m_uuidMap;  // Map Steam connections to user UUID
    std::map<boost::uuids::uuid, HSteamNetConnection> m_hConnMap;  // Map user UUIDs to Steam connection
    std::map<boost::uuids::uuid, std::string> m_nickMap;  // Map user UUIDs to username
};

/* Client Steam Socket */
struct ClientSocketSingleton : public Component<ClientSocketSingleton>
{
    ISteamNetworkingSockets* m_pInterface;  // SteamNetworkingSockets() returns raw pointer
    std::shared_ptr<SteamNetworkingConfigValue_t> opt;  // Use shared_ptr for copy constructor
    HSteamListenSocket m_hListenSock;  // uint32 handle
    HSteamNetConnection m_hConnection;  // uint32 handle
    std::shared_ptr<SteamNetworkingIPAddr> m_addrServer;
};

/* Input from a Client */
struct ClientInput : public Component<ClientInput>
{
    std::bitset<NUM_KEYBINDINGS> keyDownState;
};


/* List of entities to update for the player */
struct PlayerDelta
{

};

/* List of entities to update for ALL players */
struct WorldDeltaSingleton
{
    std::map<Entity, ComponentMask> state;
};