#pragma once

// Forward declarations
class System;
class InputManager;
struct KeyState;
template <typename T>
struct ComponentHandle;

class InputSystem : public System {
public:
    InputSystem(InputManager* inputManager);
    void init();
    void update(double dt);
    void render();

private:
    InputManager* m_inputManager;
    ComponentHandle<KeyState> keyState;
    bool pollKeys();
    void sendKeyStateToServer();
    ComponentHandle<ClientSocketSingleton> client;
};
