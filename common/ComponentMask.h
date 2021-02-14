#pragma once
#include "Component.h"

/* ComponentMask is used by Systems to identify the tuple of Components
* to iterate over, and by ClientUpdateSystem to track which Components
 need to be broadcasted to clients. 
 
 Use setDefault() to set the mask to the _default value. */

struct ComponentMask
{
    unsigned int mask = 0;
    unsigned int _default = 0; 


    // Use variadic template to accept an number of Components
    template <typename ComponentType1, typename ComponentType2, typename... Args>
    void addComponent()
    {
        mask |= (1 << GetComponentFamily<ComponentType1>());

        // Recurse
        addComponent<ComponentType2, Args...>();
    }

    // base case
    template <typename ComponentType>
    void addComponent()
    {
        mask |= (1 << GetComponentFamily<ComponentType>());
    }

    // Set default mask. Use with setDefault() to  set the mask state to the _default value.
    template <typename ComponentType1, typename ComponentType2, typename... Args>
    void addDefaultComponent()
    {
        _default |= (1 << GetComponentFamily<ComponentType1>());

        // Recurse
        addDefaultComponent<ComponentType2, Args...>();
    }

    // base case
    template <typename ComponentType>
    void addDefaultComponent()
    {
        _default |= (1 << GetComponentFamily<ComponentType>());
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

    /* Clear the mask */
    void clear()
    {
        mask = 0;
    }


    /* Clear the _default */
    void clearDefault()
    {
        _default = 0;
    }

    /* Set mask to default */
    void setDefault()
    {
        mask = _default;
    }


};
