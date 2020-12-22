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
	GameManager(std::shared_ptr<Timer> timer, InputManager* input, GraphicManager* graphic);
	~GameManager();
    void Init();                         // Initialize ECS and World
    void AddSystems();
	void Update();                       // Tick timer and update world
    World* getWorld();                   // Get reference to world

private:
    std::shared_ptr<Timer> m_timer;
    std::shared_ptr<World> m_world;      // Systems must have shared ownership of the World
    InputManager* m_inputManager;
    GraphicManager* m_graphicManager;
};
