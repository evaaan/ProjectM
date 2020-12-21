#pragma once


// Server states
namespace ServerControl
{
    enum class EngineState
    {
        WaitingForResources,
        Active,
        Terminate,
    };
};

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
};

