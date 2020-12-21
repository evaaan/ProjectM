#include "Entity.h"
#include "Component.h"
#include "System.h"
#include "ComponentHandle.h"
#include "ComponentMask.h"
#include "OverlaySystem.h"
#include "GraphicManager.h"
#include "World.h"
#include "Direct2D.h"

OverlaySystem::OverlaySystem(GraphicManager* graphicManager) : m_graphicManager(graphicManager)
{
    signature.addComponent<Outline>();
    signature.addComponent<Transform>();
}

void OverlaySystem::init()
{
}

/* Tick sprite animation frames */
void OverlaySystem::update(double dt)
{
}

/* Draw sprite bitmaps to display */
void OverlaySystem::render()
{
    for (auto& entity : registeredEntities)
    {
        /* Get animation and transform Components*/
        ComponentHandle<Outline> outline;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, outline, transform);


        D2D1_RECT_F rectangle = D2D1::RectF(
            transform->x - (outline->width) / 2,
            transform->y - (outline->width) / 2,
            transform->x + transform->width + (outline->width / 2),
            transform->y + transform->height + (outline->width / 2)
        );

        m_graphicManager->m_direct2D->deviceContext->DrawRectangle(&rectangle,
            m_graphicManager->m_direct2D->getBrush(outline->color).Get(),
            (FLOAT)outline->width);

    }
}
