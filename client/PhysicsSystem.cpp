#include "InputManager.h"
#include "System.h"
#include "PhysicsSystem.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentMask.h"
#include "Entity.h"
#include "EntityHandle.h"
#include "KeyMap.h"
#include "World.h"
#include "Direct2D.h"
#include "imgui/imgui.h"

PhysicsSystem::PhysicsSystem()
{
    signature.addComponent<Dynamic>();
    signature.addComponent<Transform>();
}


void PhysicsSystem::init()
{

}


void PhysicsSystem::update(double dt)
{
    // demoRunTick(dt);  // hacky physics demo
    // advanceTick(dt);
    // checkAllCollisions();
    // demoResolveSolidCollisions();
    // demoResolveLedgeCollisions();
    // demoUpdateTransforms();
    showImGui();
}


void PhysicsSystem::showImGui()
{
    /* Entity position, velocity, and dimensions */
    ImGui::Begin("PhyicsSystem");
    for (auto& entity : registeredEntities)
    {
        /* Get Components*/
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);
        ImGui::PushID(entity.uuid);  // Use PushID and PopID to distinguish ids
        ImGui::Text("Entity %d", entity.uuid);

        int pos2a[2] = { (int)transform->x, (int)transform->y };
        ImGui::DragInt2("Position (pix)", pos2a);
        float vel2a[2] = { (float)dynamic->vel.x, (float)dynamic->vel.y };
        ImGui::DragFloat2("Velocity (pix/sec)", vel2a);

        ImGui::PopID();
    }

    /* Collisions 
    ImGui::Begin("Collisions");
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();

    ImGui::Text("collisionMap");
    for (auto const& [entity, colSet] : colHandle->collisionMap)
    {
        ImGui::Text("%d :", entity); ImGui::SameLine();
        for (int i : colSet)
        {
            ImGui::Text("%d", i); ImGui::SameLine();
        }
        ImGui::Spacing();
    }

    ImGui::Text("solidCollisionMap");
    for (auto const& [entity, colSet] : colHandle->solidCollisionMap)
    {
        ImGui::Text("%d :", entity); ImGui::SameLine();
        for (int i : colSet)
        {
            ImGui::Text("%d", i); ImGui::SameLine();
        }
        ImGui::Spacing();
    }
    */
    ImGui::End();
}


/* Return true if one and two overlap.  */
bool PhysicsSystem::checkCollision(ComponentHandle<Dynamic> one, ComponentHandle<Dynamic> two)
{
    int one_left, one_right, two_left, two_right;
    bool collisionX = one->pos.x + one->width > two->pos.x && two->pos.x + two->width > one->pos.x;
    bool collisionY = one->pos.y + one->height > two->pos.y && two->pos.y + two->height > one->pos.y;
    return collisionX && collisionY;
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

/* Return all solid collisions for the given Entity */
std::set<int> PhysicsSystem::getSolidCollisions(Entity entity)
{
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();
    std::set<int> solidCollisions{};
    return solidCollisions;
}

/* Advance physics simulation */
void PhysicsSystem::advanceTick(double dt)
{
    // ComponentHandle<KeyState> input = parentWorld->getSingletonComponent<KeyState>();

    for (auto& entity : registeredEntities)
    {
        /* Get Components */
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);

        bool falling = false;
        double walking_speed = 300.0;
        double max_vel = -800.0;
        int floor_height = 600;
        int wall_length = 1600;

        /* Save last position */
        if (dynamic->prev_pos.y != dynamic->pos.y)
            falling = true;

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
            falling = false;
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


/* Advance the physics simulation */
void PhysicsSystem::demoRunTick(double dt)
{

    ComponentHandle<KeyState> input = parentWorld->getSingletonComponent<KeyState>();

    for (auto& entity : registeredEntities)
    {
        /* Get Components*/
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);

        bool falling = false;
        double walking_speed = 300.0;
        double max_vel = -800.0;
        int floor_height = 600;
        int wall_length = 1600;

        /* Save last position */
        if (dynamic->prev_pos.y != dynamic->pos.y)
            falling = true;

        dynamic->prev_pos = dynamic->pos;

        if (dynamic->type == BodyType::Mob)
        {

            /* Set x velocity based on key presses. */
            if (!falling)
            {
                dynamic->vel.x = 0;
                if (keyDown(input->keyDownState, MOVE_RIGHT))
                {
                    dynamic->vel.x += walking_speed;
                }
                if (keyDown(input->keyDownState, MOVE_LEFT))
                {
                    dynamic->vel.x -= walking_speed;
                }
            }

            if (keyDown(input->keyDownState, RESET) && !(falling))
            {
                dynamic->vel.y -= 800;
                falling = true;
            }

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
                falling = false;
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

/* Resolve Collisions between each Entity and it's Solid Collisions */
void PhysicsSystem::demoResolveSolidCollisions()
{
    /* Get Collision Sets */
    ComponentHandle<CollisionSingleton> colHandle = parentWorld->getSingletonComponent<CollisionSingleton>();
    for (auto const& [entityId, uuidSet] : colHandle->solidCollisionMap)
    {
        /* Get Entity Components */
        Entity entity = { entityId };
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);

        /* Resolve every collision in the set */
        for (int solidID : uuidSet)
        {
            /* Get Solid Components */
            Entity solidEntity = { solidID };
            ComponentHandle<Dynamic> solidDynamic;
            ComponentHandle<Transform> solidTransform;
            parentWorld->unpack(solidEntity, solidDynamic, solidTransform);

            // If  last frame was above the solid, stand on top edge
            if (dynamic->prev_pos.y + dynamic->height <= solidDynamic->pos.y)
            {
                dynamic->pos.y = solidDynamic->pos.y - dynamic->height;
                //dynamic->pos.x = dynamic->prev_pos.x;
                dynamic->vel.y = 0.0;
                dynamic->vel.x = 0.0;
            }
            

            // If last frame was to left of solid, translate to left edge.

            // If last frame was to right of solid, translate to right edge.

            // Else we were below the solid, translate to bottom edge.
        }
    }
}


void PhysicsSystem::demoResolveLedgeCollisions()
{/* Get Collision Sets */
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
                //dynamic->pos.x = dynamic->prev_pos.x;
                dynamic->vel.y = 0.0;
                dynamic->vel.x = 0.0;
            }


            // If last frame was to left of solid, translate to left edge.

            // If last frame was to right of solid, translate to right edge.

            // Else we were below the solid, translate to bottom edge.
        }
    }

}

/* Copy updated Dynamic positions into Transform positions to display */
void PhysicsSystem::demoUpdateTransforms()
{
    for (auto& entity : registeredEntities)
    {
        /* Get Components */
        ComponentHandle<Dynamic> dynamic;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, dynamic, transform);
        transform->x = dynamic->pos.x;
        transform->y = dynamic->pos.y;
    }
}

void PhysicsSystem::render() {}
