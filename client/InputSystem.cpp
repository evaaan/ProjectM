#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "World.h"
#include "InputManager.h"
#include "System.h"
#include "InputSystem.h"
#include "Entity.h"
#include "KeyMap.h"
#include "fbs/entity_generated.h"
#include "Utilities.h"


/* Poll keyboard state and send to server */
InputSystem::InputSystem(InputManager* inputManager) : m_inputManager(inputManager)
{
    signature.addComponent<ClientSocketSingleton>();
    signature.addComponent<KeyState>();
}

void InputSystem::init()
{
    /* Only one client socket component exists */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ClientSocketSingleton> client_component;
        ComponentHandle<KeyState> key_component;
        parentWorld->unpack(entity, key_component, client_component);

        /* Store a reference directly to the keystate component. */
        keyState = key_component;

        /* Store a reference directly to the server singleton component. */
        client = client_component;
    }
}

/*Read onKeyDown messages from InputManager, translate to Actions, and update Input component. */
void InputSystem::update(double dt)
{
    if (pollKeys())
        sendKeyStateToServer();
}

/* Return true if any key is pressed */
bool InputSystem::pollKeys()
{
    bool keyDown = false;
    keyState->keyDownState.reset();

    /* Ignore input when inactive*/
    if (m_inputManager->isActive)
    {
        /* Read keyboard state asynchronously, map to Actions, and upate component */
        for (const auto& [keyCode, action] : m_inputManager->keymap.keyCodeMap)
        {
            if (GetAsyncKeyState((int)keyCode) & 0x8000)
            {
                keyState->keyDownState.set(action);
                keyDown = true;
            }
        }
    }

    // Clear key message queue
    m_inputManager->m_msgQueue.clear();
    return keyDown;
}

void InputSystem::sendKeyStateToServer()
{
    flatbuffers::FlatBufferBuilder builder(1024);
    std::vector<uint8_t> types;
    std::vector<flatbuffers::Offset<void>> components;

    uint64_t keys = keyState->keyDownState.to_ullong();
    auto keyState = EntityBuffer::CreateKeyState(builder, keys);

    types.push_back(static_cast<uint8_t>(EntityBuffer::Component_KeyState));
    components.push_back(keyState.Union());


    // Create entity table and get offset
    auto entity_offset = EntityBuffer::CreateEntity(builder, { -1 }, // Server doesn't need entity ID
                                                    builder.CreateVector(types),
                                                    builder.CreateVector(components));

    // Finish the buffer
    EntityBuffer::FinishEntityBuffer(builder, entity_offset);

    // Get pointer and size
    uint8_t* buf = builder.GetBufferPointer();
    int buf_size = builder.GetSize();

    client->m_pInterface->SendMessageToConnection(client->m_hConnection, buf, buf_size, k_nSteamNetworkingSend_Reliable, nullptr);
    odsloga("Input message send!\n");

    // Reset data structures
    builder.Clear();
    types.clear();
    components.clear();
}

void InputSystem::render() {}
