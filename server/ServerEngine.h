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
    ServerEngine();
    ~ServerEngine();

    void Init();
    void Run();
    void Shutdown();



private:
    ServerControl::EngineState m_engineState;
    std::shared_ptr<Timer> m_timer;
    std::unique_ptr<ServerManager> m_server;
};

