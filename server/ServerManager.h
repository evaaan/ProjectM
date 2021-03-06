#pragma once
#include <memory>
#include <string>

// Forward declarations
class Timer;
class World;
enum class Color;
enum class AnimType;


class ServerManager
{
public:
    ServerManager(int port_num, int tick_ms);
    ~ServerManager();

    void Init();               // Initialize ECS
    void Process();            // Process world tick
    void Shutdown();

    World* getWorld();         // Get reference to world

private:
    void AddSystems();
    void AddEntities();
    int AddBox(int x, int y, int height, int width, Color color);
    int AddMonster(int x, int y, std::string animType);
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<World> m_world;      // Systems must have shared ownership of the World(s)
    int m_port;
    int m_tick;

};

