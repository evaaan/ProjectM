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


/* Convert uint32_t to a vector of set bit indices. */
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
    /* Store a reference directly to the server singleton component. */
    server = parentWorld->getSingletonComponent<ServerSocketSingleton>();

    /* Store a reference directly to the server singleton component. */
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
}


/* Poll new connections and broadcast units to users */
void ClientUpdateSystem::update(double dt)
{
    sendWorldUpdate();
    sendPlayerUpdates();
}

/* Send each client an update with its private Entity data that changed this tick */
void ClientUpdateSystem::sendPlayerUpdates()
{
    // not implemented
}

/*Send all clients an update with the Entity data that changed this tick
* Example Vector of Union code: https://github.com/google/flatbuffers/blob/master/tests/test.cpp#L2651-L2674 */
void ClientUpdateSystem::sendWorldUpdate()
{
    /*For every Entity that changed this tick (state->ComponentMask > 0),
    * create a FlatBuffer with its corresponding Components and send
    * to every connected client. */

    flatbuffers::FlatBufferBuilder builder(1024);
    std::vector<uint8_t> types;
    std::vector<flatbuffers::Offset<void>> components;

    for (auto& [id, componentMask] : worldDelta->state)
    {
        unsigned int mask = componentMask.mask;

        // Skip if there are no Entity updates to send
        if (mask == 0)
            continue;

        // Get components set in the worldDelta update
        std::vector<int> families = uint_to_vec(mask);

        // For each component, create another FlatBuffer table
        for (auto& family : families)
        {
            if (family == GetComponentFamily<Transform>())
            {
                ComponentHandle<Transform> c;
                parentWorld->unpack({ id }, c);
                auto transform = EntityBuffer::CreateTransform(builder, c->x, c->y, c->width, c->height);

                // Push back Component type and data to Union vector
                types.push_back(static_cast<uint8_t>(EntityBuffer::Component_Transform));
                components.push_back(transform.Union());
            }
            else if (family == GetComponentFamily<Dynamic>())
            {
                ComponentHandle<Dynamic> c;
                parentWorld->unpack({ id }, c);

                // Build Vec2 tables
                auto pos = EntityBuffer::Vec2(c->pos.x, c->pos.y);
                auto prev_pos = EntityBuffer::Vec2(c->prev_pos.x, c->prev_pos.y);
                auto vel = EntityBuffer::Vec2(c->vel.x, c->vel.y);
                auto accel = EntityBuffer::Vec2(c->accel.x, c->accel.y);

                // BodyType enum
                EntityBuffer::BodyType body_type;
                switch (c->type)
                {
                case BodyType::Mob: body_type = EntityBuffer::BodyType_Mob; break;
                case BodyType::Transparent: body_type = EntityBuffer::BodyType_Transparent; break;
                case BodyType::Ledge: body_type = EntityBuffer::BodyType_Ledge; break;
                case BodyType::Solid: body_type = EntityBuffer::BodyType_Solid; break;
                default: body_type = EntityBuffer::BodyType_Solid; break;
                }
                

                // Build FlatBuffer
                auto dynamic = EntityBuffer::CreateDynamic(builder, c->width, c->height, c->falling,
                                                           &pos, &prev_pos, &vel, &accel, body_type);
                types.push_back(static_cast<uint8_t>(EntityBuffer::Component_Dynamic));
                components.push_back(dynamic.Union());
            }
            else if (family == GetComponentFamily<Outline>())
            {
                ComponentHandle<Outline> c;
                parentWorld->unpack({ id }, c);
                EntityBuffer::Color buffer_color;
                switch (c->color)
                {
                case Color::yellow : buffer_color = EntityBuffer::Color_Yellow; break;
                case Color::red    : buffer_color = EntityBuffer::Color_Red;    break;
                case Color::green  : buffer_color = EntityBuffer::Color_Green;  break;
                case Color::blue   : buffer_color = EntityBuffer::Color_Blue;   break;
                case Color::black  : buffer_color = EntityBuffer::Color_Black;  break;
                case Color::white  : buffer_color = EntityBuffer::Color_White;  break;
                default: buffer_color = EntityBuffer::Color_Red; break;
                }
                auto outline = EntityBuffer::CreateOutline(builder, buffer_color, c->width);
                types.push_back(static_cast<uint8_t>(EntityBuffer::Component_Outline));
                components.push_back(outline.Union());
            }
            else if (family == GetComponentFamily<Player>())
            {
                ComponentHandle<Player> c;
                parentWorld->unpack({ id }, c);

                // Build username string
                auto username = builder.CreateString(c->username);

                // Build flatbuffer and push_back the table type
                auto player = EntityBuffer::CreatePlayer(builder, c->id, username);
                types.push_back(static_cast<uint8_t>(EntityBuffer::Component_Player));
                components.push_back(player.Union());
            }
            else if (family == GetComponentFamily<AnimationStore>())
            {
                ComponentHandle<AnimationStore> c;
                parentWorld->unpack({ id }, c);

                // Build vector of animation names that client should display
                std::vector<std::string> animType_vector;
                for (const auto& anim : c->store)
                {
                    animType_vector.push_back(anim);
                }

                auto animTypes = builder.CreateVectorOfStrings(animType_vector);

                // Create Animation table
                auto animation = EntityBuffer::CreateAnimation(builder, animTypes, c->direction);

                // Pushback to types and components vectors
                types.push_back(static_cast<uint8_t>(EntityBuffer::Component_Animation));
                components.push_back(animation.Union());
            }
        }
        // Create entity table and get offset
        auto entity_offset = EntityBuffer::CreateEntity(builder, id,
                                                        builder.CreateVector(types),
                                                        builder.CreateVector(components));

        // Finish the buffer
        EntityBuffer::FinishEntityBuffer(builder, entity_offset);

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

        // Reset worldDelta state
        componentMask.clear();
        
    }
}


/* System rendering */
void ClientUpdateSystem::render() {}


