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
    collisions = parentWorld->getSingletonComponent<CollisionSingleton>();
}

void PhysicsSystem::update(double dt)
{
    advanceTick(dt);
    checkAllCollisions();
    ResolveLedgeCollisions();
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
        int floor_height = 750;
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


/* Axis-aligned bounding box collision detection */
bool PhysicsSystem::checkCollision(ComponentHandle<Dynamic> one, ComponentHandle<Dynamic> two)
{
    int one_left, one_right, two_left, two_right;
    bool collisionX = one->pos.x + one->width > two->pos.x && two->pos.x + two->width > one->pos.x;
    bool collisionY = one->pos.y + one->height > two->pos.y && two->pos.y + two->height > one->pos.y;
    return collisionX && collisionY;
}


/* Return all solid collisions for the given Entity */
std::set<int> PhysicsSystem::getSolidCollisions(Entity entity)
{
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();
    std::set<int> solidCollisions{};
    return solidCollisions;
}

/* O(N^2) collision detection for ALL entities. Super inefficient! */
void PhysicsSystem::checkAllCollisions()
{
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();
    // Reset collisions at start of frame
    colHandle->collisionMap.clear();
    colHandle->ledgeCollisionMap.clear();
    colHandle->solidCollisionMap.clear();

    for (auto& entity1 : registeredEntities)
    {
        /* Get Components*/
        ComponentHandle<Dynamic> dynamic1;
        ComponentHandle<Transform> transform1;
        ComponentHandle<Outline> outline1;
        parentWorld->unpack(entity1, dynamic1, transform1, outline1);


        for (auto& entity2 : registeredEntities)
        {
            ComponentHandle<Dynamic> dynamic2;
            ComponentHandle<Transform> transform2;
            ComponentHandle<Outline> outline2;
            parentWorld->unpack(entity2, dynamic2, transform2, outline2);

            if (entity1 == entity2)
                continue;

            /* Update Collision component */
            if (checkCollision(dynamic1, dynamic2))
            {
                // Update Collision Maps
                colHandle->collisionMap[entity1.uuid].insert(entity2.uuid);
                if (dynamic2->type == BodyType::Solid)
                    colHandle->solidCollisionMap[entity1.uuid].insert(entity2.uuid);
                if (dynamic2->type == BodyType::Ledge)
                    colHandle->ledgeCollisionMap[entity1.uuid].insert(entity2.uuid);
            }
        }
    }
}

void PhysicsSystem::ResolveLedgeCollisions()
{
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();
    for (auto const& [entityId, uuidSet] : colHandle->ledgeCollisionMap)
    {
        /* Get Entity Components */
        Entity entity = { entityId };
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);

        /* Resolve every collision in the set */
        for (int ledgeID : uuidSet)
        {
            /* Get Solid Components */
            Entity solidEntity = { ledgeID };
            ComponentHandle<Dynamic> solidDynamic;
            ComponentHandle<Transform> solidTransform;
            parentWorld->unpack(solidEntity, solidDynamic, solidTransform);

            // If  last frame was above the solid, stand on top edge
            if (dynamic->prev_pos.y + dynamic->height <= solidDynamic->pos.y)
            {
                dynamic->pos.y = solidDynamic->pos.y - dynamic->height;
                // dynamic->pos.x = dynamic->prev_pos.x;
                dynamic->vel.y = 0.0;
                dynamic->vel.x = 0.0;
                dynamic->falling = false;
            }


            // If last frame was to left of solid, translate to left edge.

            // If last frame was to right of solid, translate to right edge.

            // Else we were below the solid, translate to bottom edge.
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
