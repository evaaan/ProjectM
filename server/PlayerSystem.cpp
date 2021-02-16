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
    odsloga("PlayerSystem init!");
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

        // If we started or stopped moving, change animation
    }
}

/* System rendering */
void PlayerSystem::render() {}
