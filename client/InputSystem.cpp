#include "World.h"
#include "InputManager.h"
#include "System.h"
#include "InputSystem.h"
#include "Entity.h"
#include "KeyMap.h"

InputSystem::InputSystem(InputManager* inputManager) : m_inputManager(inputManager)
{
    signature.addComponent<InputSingleton>();
}

void InputSystem::init() {}

/* Read onKeyDown messages from InputManager, translate to Actions, and update Input component */
void InputSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<InputSingleton> input;
        parentWorld->unpack(entity, input);

        input->keyDownState.reset();

        /* Read keyboard state asynchronously, map to Actions, and upate component */
        for (const auto & [ keyCode, action ] : m_inputManager->keymap.keyCodeMap)
        {
            if (GetAsyncKeyState((int)keyCode) & 0x8000)
                input->keyDownState.set(action);
        }

    }

    // Clear queue
    m_inputManager->m_msgQueue.clear();
}

void InputSystem::render() {}
