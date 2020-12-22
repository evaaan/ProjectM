#include "KeyMap.h"

KeyMap::KeyMap()
{
    // Default Key Mapping
    add(VK_LEFT, MOVE_LEFT);
    add(VK_RIGHT, MOVE_RIGHT);
    add(VK_UP, MOVE_UP);
    add(VK_DOWN, MOVE_DOWN);
    add(VK_SPACE, RESET);
}

KeyMap::~KeyMap() {}
