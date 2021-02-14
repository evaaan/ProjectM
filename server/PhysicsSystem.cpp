#include <boost/uuid/uuid.hpp>
#include "Entity.h"
#include "KeyMap.h"
#include "System.h"
#include "World.h"
#include "PhysicsSystem.h"
#include "Utilities.h"

PhysicsSystem::PhysicsSystem()
{
    signature.addComponent<Dynamic>();
    signature.addComponent<Transform>();
}


void PhysicsSystem::init()
{
    worldDelta = parentWorld->getSingletonComponent<WorldDeltaSingleton>();
    keys = parentWorld->getSingletonComponent<KeyStateSingleton>();
    server = parentWorld->getSingletonComponent<ServerSocketSingleton>();
}

void PhysicsSystem::update(double dt)
{
    advanceTick(dt);
    updateTransforms();

}

/* Advance physics simulation */
void PhysicsSystem::advanceTick(double dt)
{

    for (auto& entity : registeredEntities)
    {
        /* Get Components */
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);

        double walking_speed = 300.0;
        double max_vel = -800.0;
        int floor_height = 600;
        int wall_length = 1600;

        // Player types
        if (dynamic->type == BodyType::Mob)
        {
            // Process user input
            auto input = keys->keyDownMap[entity.uuid];

            if (!dynamic->falling)
            {
                dynamic->vel.x = 0;  // don't slide.

                // Walk left or right
                if (keyDown(input, MOVE_RIGHT))
                    dynamic->vel.x += walking_speed;
                if (keyDown(input, MOVE_LEFT))
                    dynamic->vel.x -= walking_speed;
            }

            // Jump
            if (keyDown(input, RESET) && !(dynamic->falling))
            {
                dynamic->vel.y -= 800;
                dynamic->falling = true;
            }

            /* Save last position */
            //if (dynamic->prev_pos.y != dynamic->pos.y)
            //    dynamic->falling = true;

            dynamic->prev_pos = dynamic->pos;

            /* Update velocity */
            dynamic->vel.x += dynamic->accel.x * dt;  // v = v0 + a(dt)
            dynamic->vel.y += dynamic->accel.y * dt;

            /* Update position */
            dynamic->pos.x += dynamic->vel.x * dt;  // r = r0 + v(dt)
            dynamic->pos.y += dynamic->vel.y * dt;

            /* Ceiling */
            if (dynamic->pos.y < 0)
            {
                dynamic->pos.y = 0;
                dynamic->vel.y = 0;
            }
            /* Floor */
            if (dynamic->pos.y > floor_height)
            {
                dynamic->pos.y = floor_height;
                dynamic->vel.y = 0;
                dynamic->falling = false;
            }

            /* Walls */
            if (dynamic->pos.x < 0)
                dynamic->pos.x = 0;
            if (dynamic->pos.x > wall_length)
                dynamic->pos.x = wall_length;

            /* Limit fall speed */
            if (dynamic->vel.y < max_vel)
                dynamic->vel.y = max_vel;
        }
    }
}

/* Copy updated Dynamic positions into Transform positions to display */
void PhysicsSystem::updateTransforms()
{
    for (auto& entity : registeredEntities)
    {
        /* Get Components */
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);
        transform->x = dynamic->pos.x;
        transform->y = dynamic->pos.y;

        // Tag Components for update
        worldDelta->state[entity.uuid].addComponent<Dynamic>();
        worldDelta->state[entity.uuid].addComponent<Transform>();
    }
}


void PhysicsSystem::render() {}
