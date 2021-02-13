#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "EntityHandle.h"
#include "System.h"
#include "World.h"
#include "ServerUpdateSystem.h"
#include "Utilities.h"
#include "fbs/entity_generated.h"


/* Receive entity updates from Server */
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
    auto entity_buffer = EntityBuffer::GetEntity(buf_ptr);
    auto id = entity_buffer->id();
    Entity e;
    e.uuid = id;

    // Create Entity if it doesn't exist
    // Will need to determine read Component Types from FlatBuffer and add them
    if (!parentWorld->entityExists(e))
    {
        // auto entity = parentWorld->createEntity(id);
        // parentWorld->createEntity(id).addComponent(Transform());
        createEntity(entity_buffer);
        odsloga("Creating a new entity, id (" << id << ") \n");
    }
    else { odsloga("Entity already exists, id (" << id << ") \n"); }
    
    updateEntity(entity_buffer);
}

/* Create an Entity based on a FlatBuffer message */
void ServerUpdateSystem::createEntity(const EntityBuffer::Entity* entity_buffer)
{
    auto cs = entity_buffer->component();        // components
    auto cts = entity_buffer->component_type();  // component types
    auto num_cts = cts->size();      // number of components

    auto id = entity_buffer->id();
    auto entity = parentWorld->createEntity(id);

    for (auto idx = 0; idx < num_cts; idx++)
    {
        auto type = cts->GetEnum<EntityBuffer::Component>(idx); // Get Component type
        switch (type)
        {
        case EntityBuffer::Component_Transform:
        { entity.addComponent(Transform()); break; }
        case EntityBuffer::Component_Dynamic:
        { entity.addComponent(Dynamic()); break; }
        case EntityBuffer::Component_KeyState:
        { entity.addComponent(KeyState()); break; }
        case EntityBuffer::Component_Animation:
        { entity.addComponent(Animation()); break; }
        case EntityBuffer::Component_Outline:
        { entity.addComponent(Outline()); break; }
        case EntityBuffer::Component_Connection:
        { break; }
        }
    }
}

/* Update an Entity based on a FlatBuffer message */
void ServerUpdateSystem::updateEntity(const EntityBuffer::Entity* entity_buffer)
{
    auto cs = entity_buffer->component();        // components
    auto cts = entity_buffer->component_type();  // component types
    auto num_cts = cts->size();      // number of components

    Entity e = { entity_buffer->id() };

    for (auto idx = 0; idx < num_cts; idx++)
    {
        auto type = cts->GetEnum<EntityBuffer::Component>(idx); // Get Component type
        switch (type)
        {
            // Transform
        case EntityBuffer::Component_Transform:
        {
            odsloga("Transform message!\n");
            ComponentHandle<Transform> transform;
            parentWorld->unpack(e, transform);

            auto data = cs->GetAs<EntityBuffer::Transform>(idx);
            transform->x = data->x();
            transform->y = data->y();
            transform->width = data->width();
            transform->height = data->height();
            break;
        }
        case EntityBuffer::Component_Dynamic:
        {
            odsloga("Dynamic message!\n");
            ComponentHandle<Dynamic> dynamic;
            parentWorld->unpack(e, dynamic);
            break;
        }
        case EntityBuffer::Component_Connection:
        {
            odsloga("Connection message!\n");
            // Assign our own nickname and which entity we control
            break;
        }
        case EntityBuffer::Component_Outline:
        {
            odsloga("Outline message!\n");
            ComponentHandle<Outline> outline;
            parentWorld->unpack(e, outline);

            auto data = cs->GetAs<EntityBuffer::Outline>(idx);
            switch (data->color())
            {
            case EntityBuffer::Color_Yellow: outline->color = Color::yellow; break;
            case EntityBuffer::Color_Red: outline->color = Color::red; break;
            case EntityBuffer::Color_Green: outline->color = Color::green; break;
            case EntityBuffer::Color_Blue: outline->color = Color::blue; break;
            case EntityBuffer::Color_Black: outline->color = Color::black; break;
            case EntityBuffer::Color_White: outline->color = Color::white; break;
            default: outline->color = Color::red; break;
            }
            outline->width = data->width();
            break;
        }

        case EntityBuffer::Component_KeyState:
        { }
        case EntityBuffer::Component_Animation:
        { }

        default:
        {
            odsloga("Unknown message! \n");
            throw std::runtime_error("Component type undefined! How do I unpack this message?\n");
        }
        }
    }

}

/* System rendering */
void ServerUpdateSystem::render() {}
