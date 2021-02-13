#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "fbs/entity_generated.h"
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ClientInputSystem.h"
#include "Component.h"
#include "Utilities.h"

/* Receive input messages from Client */
ClientInputSystem::ClientInputSystem()
{
    signature.addComponent<ServerSocketSingleton>();
    signature.addComponent<KeyStateSingleton>(); // Map of client to key state
}

void ClientInputSystem::init()
{
    /* Only one server per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSocketSingleton> server_component;
        ComponentHandle<KeyStateSingleton> keystate_component;
        parentWorld->unpack(entity, server_component, keystate_component);

        /* Store a reference directly to the server singleton component. */
        server = server_component;
        keys = keystate_component;
    }
}

/* System behaviors */
void ClientInputSystem::update(double dt)
{
    // Read messages on poll group
    int num_calls = 0;
    ISteamNetworkingMessage* pIncomingMsg[16];
    int numMsgs = server->m_pInterface->ReceiveMessagesOnPollGroup(server->m_hPollGroup, pIncomingMsg, 16);
    if (numMsgs < 0)
        throw std::runtime_error("Invalid connection handle!");
    if (numMsgs > 0)
    {
        for (int msg_num = 0; msg_num < numMsgs; msg_num++)
        {
            // Get connected user UUID and entity ID
            auto uuid = server->m_uuidMap[pIncomingMsg[msg_num]->m_conn];
            auto entity_id = server->m_idMap[uuid];

            uint8_t* buffer_pointer = (uint8_t*)pIncomingMsg[msg_num]->m_pData;
            
            // Process the message
            auto entity_buffer = EntityBuffer::GetEntity(buffer_pointer);
            processInputMessage(entity_buffer, { entity_id }, uuid);
            num_calls++;

            pIncomingMsg[msg_num]->Release();
        }
    }
}

/* Pull key state data from flatbuffer message */
void ClientInputSystem::processInputMessage(const EntityBuffer::Entity* entity_buffer, Entity e, boost::uuids::uuid uuid)
{
    auto cs = entity_buffer->component();
    auto cts = entity_buffer->component_type();
    auto num_cts = cts->size();
    /* Process all Components in Flatbuffer message */
    for (auto idx = 0; idx < num_cts; idx++)
    {
        auto type = cts->GetEnum<EntityBuffer::Component>(idx);
        switch (type)
        {
        case EntityBuffer::Component_KeyState:
        {
            // Assign fbs bitset to client's stored key state
            auto data = cs->GetAs<EntityBuffer::KeyState>(idx);
            keys->keyDownMap[uuid] = std::bitset<64>(data->bitset());

            odsloga("Client input (" << to_string(uuid) << "): " << keys->keyDownMap[uuid].to_string() << "\n");
            break;
        }
        }

    }
}

/* System rendering */
void ClientInputSystem::render() {}
