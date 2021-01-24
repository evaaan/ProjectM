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

/* System behaviors */
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
            // std::string nick((char*)pIncomingMsg[num]->m_pData, pIncomingMsg[num]->m_cbSize);
            odsloga("Received message \n");

            // Read data from the Socket
            uint8_t* buffer_pointer = (uint8_t *)pIncomingMsg[num]->m_pData;

            // Get a pointer to the root object inside the buffer
            auto entity = GetEntityFbs(buffer_pointer);

            // Get entity to update
            auto id = entity->id();

            // Unpack message components
            auto cs = entity->data();        // components
            auto cts = entity->data_type();  // component types
            auto num_cts = cts->size();      // number of components

            for (int idx = 0; idx < num_cts; idx++)
            {
                auto type = cts->GetEnum<Data>(idx);
                switch (type)
                {
                case Data_TransformFbs:
                    auto transform = cs->GetAs<TransformFbs>(idx);
                    auto x = transform->x();
                    auto y = transform->y();
                    auto width = transform->width();
                    auto height = transform->height();
                    odsloga("id: " << id << ", x: " << x << ", y: " << y << ", width: " << width << ", height: " << height << "\n");

                    ComponentHandle<Dynamic> dynamic_component;
                    Entity e;
                    e.uuid = id;
                    parentWorld->unpack(e, dynamic_component);
                    dynamic_component->pos.y = y;
                    break;
                }
            }

            pIncomingMsg[num]->Release();
        }
    }
}

/* System rendering */
void ServerUpdateSystem::render() {}
