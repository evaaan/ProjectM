#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>
#include <windows.h>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>

#include "fbs/entity_generated.h"
#include "Entity.h"
#include "System.h"
#include "World.h"
#include "ClientUpdateSystem.h"
#include "Utilities.h"

using namespace EntityBuffer;  // FlatBuffer

std::vector<int> uint_to_vec(unsigned int val)
{
    std::vector<int> result;
    std::bitset<32> bits(val);

    for (int index = 0; index < 32; index++)
    {
        if (bits.test(index))
            result.push_back(index);
    }
    return result;

}

ClientUpdateSystem::ClientUpdateSystem()
{
    // Add ComponentTypes the System acts on
    signature.addComponent<ServerSocketSingleton>();
    signature.addComponent<WorldDeltaSingleton>();
}


void ClientUpdateSystem::init()
{

    /* Only one server per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSocketSingleton> server_component;
        parentWorld->unpack(entity, server_component);

        /* Store a reference directly to the server singleton component. */
        server = server_component;
    }

    /* Only one global delta per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<WorldDeltaSingleton> delta_component;
        parentWorld->unpack(entity, delta_component);

        /* Store a reference directly to the server singleton component. */
        worldDelta = delta_component;
    }
}


/* Poll new connections and broadcast units to users */
void ClientUpdateSystem::update(double dt)
{
    UpdateAllClients();
    sendWorldUpdate();
    sendPlayerUpdates();
}

/* Send each client an update with its private Entity data that changed this tick */
void ClientUpdateSystem::sendPlayerUpdates()
{
    // not implemented
}

/* Send all clients an update with the Entity data that changed this tick */
void ClientUpdateSystem::sendWorldUpdate()
{
    /*For every Entity that changed this tick (state->ComponentMask > 0),
    * create a FlatBuffer with its corresponding Components and send
    * to every connected client. */


    flatbuffers::FlatBufferBuilder builder(1024);
    std::vector<uint8_t> types;
    std::vector<flatbuffers::Offset<void>> components;

    for (auto const& [entity, cm] : worldDelta->state)
    {
        int id = entity.uuid;
        unsigned int mask = cm.mask;

        // Create an EntityFbs FlatBuffer with all components found in mask
        // I have a uint32 0b0110. Player=2, Transform=3, so make those components.
        std::vector<int> families = uint_to_vec(mask);

        for (auto const& family : families)
        {
            if (family == GetComponentFamily<Transform>())
            {

            }
            else if (family == GetComponentFamily<Dynamic>())
            {
            }
        }

        // Get all Component types from unint32 mask

        // for each Component type, create a table and push it back


        // Create entity table and get offset
        auto entity_offset = CreateEntityFbs(builder, id,
            builder.CreateVector(types),
            builder.CreateVector(components));

        // Finish the buffer
        FinishEntityFbsBuffer(builder, entity_offset);

        // Get pointer and size
        uint8_t* buf = builder.GetBufferPointer();
        int buf_size = builder.GetSize();

        // Send to all clients
        for (auto const& [hConn, uuid] : server->m_uuidMap)
        {
            server->m_pInterface->SendMessageToConnection(hConn, buf, buf_size, k_nSteamNetworkingSend_Reliable, nullptr);
        }

        // Reset data structures
        builder.Clear();
        types.clear();
        components.clear();
    }
}

/* Loop over player connections, package and send a message for each WorldDelta entity update */
void ClientUpdateSystem::UpdateAllClients()
{
    flatbuffers::FlatBufferBuilder builder(1024);

    // Components
    int x = 600;
    int y = 400;
    int width = 300;
    int height = 200;
    auto transform = CreateTransformFbs(builder, x, y, width, height);

    // Example Vector of Union code: https://github.com/google/flatbuffers/blob/master/tests/test.cpp#L2651-L2674 
    // union types
    std::vector<uint8_t> types;
    types.push_back(static_cast<uint8_t>(Data_TransformFbs));

    // union values
    std::vector<flatbuffers::Offset<void>> components;
    components.push_back(transform.Union());

    // create EntityFbs
    int id = 4;
    auto entity_offset = CreateEntityFbs(builder, id,
                                         builder.CreateVector(types),
                                         builder.CreateVector(components));

    FinishEntityFbsBuffer(builder, entity_offset);

    uint8_t* buf = builder.GetBufferPointer();
    int buf_size = builder.GetSize();

    for (auto const& [hConn, uuid] : server->m_uuidMap)
    {
        server->m_pInterface->SendMessageToConnection(hConn, buf, buf_size, k_nSteamNetworkingSend_Reliable, nullptr);
    }
}


/* System rendering */
void ClientUpdateSystem::render() {}


