#pragma once
#include <memory>



// Server states
namespace ServerControl
{
    enum class EngineState
    {
        Startup,
        Active,
        Terminate,
        OneShot,
    };
};

// Forward declarations
class Timer;
class ServerManager;

class ServerEngine
{
public:
    ServerEngine(int port_num, int tick_ms);
    ~ServerEngine();

    void Run();
    void Shutdown();

private:
    // ServerControl::EngineState m_engineState;
    std::unique_ptr<ServerManager> m_server;
    int m_port; // port number
    int m_tick; // tick ms
};

