#include "Entity.h"
#include "System.h"
#include "World.h"
#include "PlayerSystem.h"
#include "Utilities.h"

PlayerSystem::PlayerSystem()
{
    // Components that define a player
    signature.addComponent<AnimationStore, Transform, Dynamic, Outline>();
}

void PlayerSystem::init()
{
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
    keys = parentWorld->getSingletonComponent<KeyStateSingleton>();
}

/* System behaviors */
void PlayerSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<AnimationStore> animationStore;
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Player> player;
        parentWorld->unpack(entity, animationStore, dynamic, player);

        // If direction changed, notify client
        bool old_direction = animationStore->direction;
        bool new_direction = dynamic->direction;

        if (new_direction != old_direction)
        {
            animationStore->direction = new_direction;
            worldDelta->state[entity.uuid].addComponent<AnimationStore>();
        }

        // If we started attacking, play Attack animation
        if (player->start_combat)
        {
            animationStore->store.clear();
            animationStore->store.insert(AnimType::Attack);
            worldDelta->state[entity.uuid].addComponent<AnimationStore>();
        }

        // If we stopped moving, play Idle animation
        else if (dynamic->stop_move)
        {
            animationStore->store.clear();
            animationStore->store.insert(AnimType::Idle);
            worldDelta->state[entity.uuid].addComponent<AnimationStore>();
        } // If we started moving, play Walk animation
        else if (dynamic->start_move)
        {
            animationStore->store.clear();
            animationStore->store.insert(AnimType::Walk);
            worldDelta->state[entity.uuid].addComponent<AnimationStore>();
        }


    }
}

/* Update player status based on user actions */
void PlayerSystem::updatePlayer()
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<AnimationStore> animationStore;
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Player> player;
        parentWorld->unpack(entity, animationStore, dynamic, player);

        auto input = keys->keyDownMap[entity.uuid];

        if (keyDown(input, ATTACK))
        {
            // set player attacking state such that animationsystem can update client
        }

    }
}

/* System rendering */
void PlayerSystem::render() {}
