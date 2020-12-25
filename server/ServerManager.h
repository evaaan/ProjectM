#pragma once
#include <memory>

// Forward declarations
class Timer;
class World;


class ServerManager
{
public:
    ServerManager(std::shared_ptr<Timer> timer);
    ~ServerManager();

    void Init();               // Initialize ECS
    void Process();            // Process world tick

    World* getWorld();         // Get reference to world

private:
    void AddSystems();
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<World> m_world;      // Systems must have shared ownership of the World(s)

};

