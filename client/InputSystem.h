#pragma once

// Forward declarations
class System;
class InputManager;

class InputSystem : public System {
public:
    InputSystem(InputManager* inputManager);
    void init();
    void update(double dt);
    void render();

private:
    InputManager* m_inputManager;
};
