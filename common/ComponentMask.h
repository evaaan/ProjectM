#pragma once
#include "Component.h"

/* ComponentMask is used by Systems to identify the tuple of Components
* to iterate over. */

struct ComponentMask
{
    unsigned int mask = 0;

    template <typename ComponentType>
    void addComponent()
    {
        mask |= (1 << GetComponentFamily<ComponentType>());
    }

    /* Remove ComponentType from Mask*/
    template <typename ComponentType>
    void removeComponent()
    {
        mask &= ~(1 << GetComponentFamily<ComponentType>());
    }


    /* Returns true if system is now matched but didn't used to be */
    bool isNewMatch(ComponentMask oldMask, ComponentMask systemMask)
    {
        return matches(systemMask) && !oldMask.matches(systemMask);
    }

    /* Returns true if the system is not matched, but used to be matched */
    bool isNoLongerMatched(ComponentMask oldMask, ComponentMask systemMask)
    {
        return oldMask.matches(systemMask) && !matches(systemMask);
    }

    /* Returns true if systemMask matches our mask */
    bool matches(ComponentMask systemMask)
    {
        return ((mask & systemMask.mask) == systemMask.mask);
    }


};
