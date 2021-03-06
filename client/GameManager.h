#pragma once
#include <memory>

// forward declarations
class App;
class World;
class InputManager;
class GraphicManager;

class GameManager
{
public:
	GameManager(std::shared_ptr<Timer> timer, InputManager* input, GraphicManager* graphic, int tick_ms);
	~GameManager();
    void Init();                         // Initialize ECS and World
    void AddSystems();
    void AddEntities();
	double Update();                       // Tick timer and update world
    World* getWorld();                   // Get reference to world

private:
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<World> m_world;      // Systems must have shared ownership of the World
    InputManager* m_inputManager;
    GraphicManager* m_graphicManager;
    int m_tick; // ms


    /* Create entities */
    void addWolf(int x, int y);
    void addBox(int x, int y);
};
