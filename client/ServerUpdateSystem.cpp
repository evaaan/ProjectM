#include <stdexcept>
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include "EntityHandle.h"
#include "System.h"
#include "World.h"
#include "GraphicManager.h"
#include "ServerUpdateSystem.h"
#include "Utilities.h"
#include "fbs/entity_generated.h"


/* Receive entity updates from Server */
ServerUpdateSystem::ServerUpdateSystem(GraphicManager* graphicManager) : m_graphicManager(graphicManager)
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

    odsloga("New entity (" << id << ") has " << num_cts << " components.\n");
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
        { entity.addComponent(AnimationStore()); break; }
        case EntityBuffer::Component_Outline:
        { entity.addComponent(Outline()); break; }
        case EntityBuffer::Component_Player:
        { entity.addComponent(Player());  break; }
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

            auto data = cs->GetAs<EntityBuffer::Dynamic>(idx);
            dynamic->width = data->width();
            dynamic->height = data->height();
            dynamic->falling = data->falling();
            dynamic->pos.x = data->pos()->x();
            dynamic->pos.y = data->pos()->y();
            dynamic->prev_pos.x = data->prev_pos()->x();
            dynamic->prev_pos.y = data->prev_pos()->y();
            dynamic->vel.x = data->vel()->x();
            dynamic->vel.y = data->vel()->y();
            dynamic->accel.x = data->accel()->x();
            dynamic->accel.y = data->accel()->y();

            switch (data->type())
            {
            case EntityBuffer::BodyType_Mob: dynamic->type = BodyType::Mob; break;
            case EntityBuffer::BodyType_Transparent: dynamic->type = BodyType::Transparent; break;
            case EntityBuffer::BodyType_Ledge: dynamic->type = BodyType::Ledge; break;
            case EntityBuffer::BodyType_Solid: dynamic->type = BodyType::Solid; break;
            default: dynamic->type = BodyType::Solid; break;
            }
            break;
        }
        case EntityBuffer::Component_KeyState:
        {  // Won't get a KeyState from the server
            break;
        }

        case EntityBuffer::Component_Player:
        {
            odslog("Player message!\n");
            ComponentHandle<Player> player;
            parentWorld->unpack(e, player);

            auto data = cs->GetAs<EntityBuffer::Player>(idx);
            player->id = data->id();
            player->username = data->username()->str();
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
        case EntityBuffer::Component_Animation:
        {
            odsloga("Animation message!\n");
            ComponentHandle<AnimationStore> animation;
            parentWorld->unpack(e, animation);

            auto data = cs->GetAs<EntityBuffer::Animation>(idx);

            // Clear animation store
            animation->store.clear();

            // Unpack AnimTypes from flatbuffer
            animation->direction = data->direction(); // true for right-facing, false for left-facing
            auto animTypes = data->animType();        // A pointer to a 'flatbuffers::Vector<>'
            auto anims_size = animTypes->size();
            for (int num_anim=0; num_anim < anims_size; num_anim++)
            {
                switch (animTypes->Get(num_anim))
                {
                case EntityBuffer::AnimType_Idle: animation->store.insert(AnimType::Idle); break;
                case EntityBuffer::AnimType_Walk: animation->store.insert(AnimType::Walk); break;
                case EntityBuffer::AnimType_Attack: animation->store.insert(AnimType::Attack); break;
                case EntityBuffer::AnimType_Fall: animation->store.insert(AnimType::Fall); break;
                default: animation->store.insert(AnimType::Idle); break;
                }
            }

            // Load these from configs!
            Animation idleAnimation;
            idleAnimation.animationFPS = 2.0;
            Animation walkAnimation;
            walkAnimation.animationFPS = 6.0;

            // Load animation if we haven't seen it already. Move this to the switch statement?
            if (animation->animations.find(AnimType::Idle) == animation->animations.end()) // not found
            {
                m_graphicManager->loadAnimation(L"../../assets/sprites/nakedMan.png", idleAnimation);
                animation->animations[AnimType::Idle] = idleAnimation; // copy constructor
                odsloga("assets loaded\n");
            }
            if (animation->animations.find(AnimType::Walk) == animation->animations.end()) // not found
            {
                m_graphicManager->loadAnimation(L"../../assets/sprites/nakedManWalk.png", walkAnimation);
                animation->animations[AnimType::Walk] = walkAnimation; // copy constructor
            }
            break;
        }
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
