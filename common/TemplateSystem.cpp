#include "Entity.h"
#include "System.h"
#include "World.h"
#include "TemplateSystem.h"

TemplateSystem::TemplateSystem()
{
    // Add ComponentTypes the System acts on
    // signature.addComponent<ComponentType>();
}

void TemplateSystem::init() {}

/* System behaviors */
void TemplateSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        ComponentHandle<KeyState> input;
        parentWorld->unpack(entity, input);
    }

}

/* System rendering */
void TemplateSystem::render() {}
