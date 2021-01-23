#include <chrono>
#include <random>
#include <stdexcept>
#include <thread>
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


void UpdateClientSystem::UpdateAllClients()
{
    // Create a buffer
    flatbuffers::FlatBufferBuilder builder(1024);
    int x = 600;
    int y = 400;
    int width = 300;
    int height = 200;
    auto transform = CreateTransformFbs(builder, x, y, width, height);  // Component
    EntityFbsBuilder entity_builder(builder);
    entity_builder.add_id(4); // entity id
    entity_builder.add_data_type(Data_TransformFbs);
    entity_builder.add_data(transform.Union());
    auto entity = entity_builder.Finish();
    builder.Finish(entity);

    uint8_t* buf = builder.GetBufferPointer();
    int buf_size = builder.GetSize();

    for (auto const& [hConn, uuid] : server->m_uuidMap)
    {
        server->m_pInterface->SendMessageToConnection(hConn, buf, buf_size, k_nSteamNetworkingSend_Reliable, nullptr);
    }
}


/* System rendering */
void UpdateClientSystem::render() {}


