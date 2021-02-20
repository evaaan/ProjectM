#include "Entity.h"
#include "System.h"
#include "World.h"
#include "PlayerSystem.h"
#include "Utilities.h"

PlayerSystem::PlayerSystem()
{
    // Components that define a player
    signature.addComponent<AnimationStore, Player, Combat, Transform, Dynamic, Outline>();
}

void PlayerSystem::init()
{
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
    keys = parentWorld->getSingletonComponent<KeyStateSingleton>();
}

/* System behaviors */
void PlayerSystem::update(double dt)
{
    updatePlayerState();
    updatePlayerAnimation();
}

/* Update animation based on player state */
void PlayerSystem::updatePlayerAnimation()
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
        if (player->start_combat)  // set in updatePlayerState
        {
            animationStore->store.clear();
            animationStore->store.insert(AnimType::Attack);
            animationStore->store.insert(AnimType::WeaponAttack);
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
void PlayerSystem::updatePlayerState()
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<Player> player;
        parentWorld->unpack(entity, player);

        auto input = keys->keyDownMap[entity.uuid];

        if (keyDown(input, ATTACK))
        {
            // set player attacking state so that animationsystem can update client
            if (player->attacking)
                player->start_combat = false;
            else
                player->start_combat = true;
            player->attacking = true;
            player->in_combat = true;
        }
        else 
        {
            player->attacking = false;
            player->start_combat = false;
            player->in_combat = false;
        }
    }
}

/* System rendering */
void PlayerSystem::render() {}
