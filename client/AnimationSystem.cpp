#include "System.h"
#include "AnimationSystem.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentMask.h"
#include "Entity.h"
#include "InputManager.h"
#include "GraphicManager.h"
#include "KeyMap.h"
#include "World.h"
#include "Animation.h"
#include "Direct2D.h"

AnimationSystem::AnimationSystem(GraphicManager* graphicManager) : m_graphicManager(graphicManager)
{
    signature.addComponent<Animation>();
    signature.addComponent<Transform>();
}

void AnimationSystem::init() 
{
    for (auto& entity : registeredEntities)
    {
        /* Set initial Animation states */
        ComponentHandle<Animation> animation;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, animation, transform);
        m_graphicManager->loadAnimation(L"C:/Users/Evan/source/repos/GameAppWin32/Art/nakedManMirror.png", animation);
    }
}

/* Tick sprite animation frames */
void AnimationSystem::update(double dt)
{
    for (auto& entity : registeredEntities)
    {
        /* Get Components*/
        ComponentHandle<Animation> animation;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, animation, transform);

        animation->frameTime += dt;

        // check whether it is time to change to another frame
        if (animation->frameTime > (1.0 / animation->animationFPS))
        {
            // the number of frames to increment is the integral result of frameTime / (1 / animationFPS), thus frameTime * animationFPS
            animation->activeAnimationFrame += (unsigned int)(animation->frameTime * animation->animationFPS);

            // use modulo computation to make sure to not jump past the last frame
            if (animation->activeAnimationFrame >= animation->cyclesData[animation->activeAnimation].numberOfFrames)
               animation->activeAnimationFrame = animation->activeAnimationFrame % animation->cyclesData[animation->activeAnimation].numberOfFrames;
        }

        // set the frame time
        animation->frameTime = std::fmod(animation->frameTime, 1.0 / (double)animation->animationFPS);
    }
}

/* Draw sprite bitmaps to display */
void AnimationSystem::render()
{
    for (auto& entity : registeredEntities)
    {
        /* Get animation and transform Components*/
        ComponentHandle<Animation> animation;
        ComponentHandle<Transform> transform;
        parentWorld->unpack(entity, animation, transform);

        unsigned int cycle = animation->activeAnimation;
        unsigned int frame = animation->activeAnimationFrame;
        AnimationCycle cycleData = animation->cyclesData[cycle];

        // compute the render target location
        D2D1_RECT_F destRect = { transform->x,
                                transform->y,
                                transform->x + cycleData.destWidth,
                                transform->y + cycleData.destHeight };


        // get next frame in the sprite sheet
        float startX = frame * (cycleData.width + cycleData.paddingWidth) + cycleData.borderPaddingWidth;
        float startY = 0;
        for (unsigned int i = 0; i < cycle; i++)
            startY += (animation->cyclesData[i].height + animation->cyclesData[i].paddingHeight);
        startY += animation->cyclesData[0].borderPaddingHeight;
        D2D1_RECT_F sourceRect = { startX, startY, startX + cycleData.width, startY + cycleData.height };

        m_graphicManager->drawBitmap(animation->bitmap.Get(), destRect, animation->opacity, animation->interpol, sourceRect);

    }
}
