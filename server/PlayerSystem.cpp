#include "Entity.h"
#include "System.h"
#include "World.h"
#include "PlayerSystem.h"
#include "Utilities.h"

PlayerSystem::PlayerSystem()
{
    // Components that define a player
    signature.addComponent<AnimationStore, Transform, Dynamic, Outline, Player>();
}

void PlayerSystem::init()
{
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
}

/* System behaviors */
void PlayerSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<AnimationStore> animationStore;
        ComponentHandle<Dynamic> dynamic;
        parentWorld->unpack(entity, animationStore, dynamic);

        // If direction changed, notify client
        bool old_direction = animationStore->direction;
        bool new_direction = dynamic->direction;

        if (new_direction != old_direction)
        {
            animationStore->direction = new_direction;
            worldDelta->state[entity.uuid].addComponent<AnimationStore>();
        }

        // If we stopped moving, play Idle animation
        if (dynamic->stop_move)
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

/* System rendering */
void PlayerSystem::render() {}
