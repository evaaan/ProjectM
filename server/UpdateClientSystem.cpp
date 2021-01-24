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
#include "UpdateClientSystem.h"
#include "Utilities.h"

using namespace EntityBuffer;  // FlatBuffer

UpdateClientSystem::UpdateClientSystem()
{
    // Add ComponentTypes the System acts on
    signature.addComponent<ServerSocketSingleton>();
}


void UpdateClientSystem::init()
{

    /* Only one server per World */
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<ServerSocketSingleton> server_component;
        parentWorld->unpack(entity, server_component);

        /* Store a reference directly to the server singleton component. */
        server = server_component;
    }
}


/* Poll new connections and broadcast units to users */
void UpdateClientSystem::update(double dt)
{
    UpdateAllClients();
}

/* Loop over player connections, package and send a message for each WorldDelta entity update */
void UpdateClientSystem::UpdateAllClients()
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
void UpdateClientSystem::render() {}


