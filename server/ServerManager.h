#pragma once
#include <memory>

// Forward declarations
class Timer;
class World;
enum class Color;


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
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<World> m_world;      // Systems must have shared ownership of the World(s)
    int m_port;
    int m_tick;

};

