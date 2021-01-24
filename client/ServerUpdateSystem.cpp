#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ServerUpdateSystem.h"
#include "Utilities.h"
#include "fbs/entity_generated.h"

using namespace EntityBuffer;  // FlatBuffer

ServerUpdateSystem::ServerUpdateSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
    signature.addComponent<ClientSocketSingleton>();
}

void ServerUpdateSystem::init()
{
    /* Only one client socket component exists */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ClientSocketSingleton> client_component;
        parentWorld->unpack(entity, client_component);

        /* Store a reference directly to the client singleton component. */
        client = client_component;
    }
}

/* Read and process server messages */
void ServerUpdateSystem::update(double dt)
{
    // Read incoming messages into an ISteamNetworkingMessage array
    ISteamNetworkingMessage* pIncomingMsg[8];
    int numMsgs = client->m_pInterface->ReceiveMessagesOnConnection(client->m_hConnection, pIncomingMsg, 8);
    if (numMsgs < 0)
        throw std::runtime_error("Invalid connection handle!");
    for (int num = 0; num < numMsgs; num++)
    {
        if (pIncomingMsg != nullptr && pIncomingMsg[num] != nullptr)
        {
            odsloga("Received message \n");

            // Read data from the Socket
            uint8_t* buffer_pointer = (uint8_t *)pIncomingMsg[num]->m_pData;

            // Process the message
            processFbMessage(buffer_pointer);

            pIncomingMsg[num]->Release();
        }
    }
}

/* Takes a pointer to a FlatBuffer buffer, unpacks the data,
 and updates the corresponding Entity */
void ServerUpdateSystem::processFbMessage(uint8_t* buf_ptr)
{
    // Get a pointer to the root object inside the buffer
    auto entity = GetEntityFbs(buf_ptr);

    // Get entity
    auto id = entity->id();

    // Unpack message
    auto cs = entity->data();        // components
    auto cts = entity->data_type();  // component types
    auto num_cts = cts->size();      // number of components

    Entity e;
    e.uuid = id;

    for (int idx = 0; idx < num_cts; idx++)
    {
        auto type = cts->GetEnum<Data>(idx);
        switch (type)
        {
            // Transform
            case Data_TransformFbs:
            {
                ComponentHandle<Transform> transform;
                parentWorld->unpack(e, transform);

                auto data = cs->GetAs<TransformFbs>(idx);
                transform->x = data->x();
                transform->y = data->y();
                transform->width = data->width();
                transform->height = data->height();
                break;
            }
            case Data_DynamicFbs:
            {
                ComponentHandle<Dynamic> dynamic;
                parentWorld->unpack(e, dynamic);
                break;
            }

            default:
                throw std::runtime_error("Component type undefined! How do I unpack this message?\n");
        }
    }

}

/* System rendering */
void ServerUpdateSystem::render() {}
