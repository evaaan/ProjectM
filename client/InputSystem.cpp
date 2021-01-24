
#include "World.h"
#include "InputManager.h"
#include "System.h"
#include "InputSystem.h"
#include "Entity.h"
#include "KeyMap.h"
#include "fbs/entity_generated.h"

/* Poll keyboard state and send to server */
InputSystem::InputSystem(InputManager* inputManager) : m_inputManager(inputManager)
{
    signature.addComponent<KeyState>();
}

void InputSystem::init()
{
    /* Only one client socket component exists */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<KeyState> key_component;
        parentWorld->unpack(entity, key_component);

        /* Store a reference directly to the client singleton component. */
        keyState = key_component;
    }
}

/*Read onKeyDown messages from InputManager, translate to Actions, and update Input component. */
void InputSystem::update(double dt)
{
    pollKeys();
    sendKeyStateToServer();
}

void InputSystem::pollKeys()
{
        keyState->keyDownState.reset();

        /* Ignore input when inactive*/
        if (m_inputManager->isActive)
        {
            /* Read keyboard state asynchronously, map to Actions, and upate component */
            for (const auto& [keyCode, action] : m_inputManager->keymap.keyCodeMap)
            {
                if (GetAsyncKeyState((int)keyCode) & 0x8000)
                    keyState->keyDownState.set(action);
            }
        }
    // Clear queue
    m_inputManager->m_msgQueue.clear();
}

void InputSystem::sendKeyStateToServer()
{

}

void InputSystem::render() {}
