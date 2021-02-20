#include "KeyMap.h"

KeyMap::KeyMap()
{
    WPARAM KEY_A = 0x41;
    WPARAM KEY_S = 0x53;
    WPARAM KEY_D = 0x44;
    WPARAM KEY_F = 0x46;

    // Default Key Mapping
    add(VK_LEFT, MOVE_LEFT);
    add(VK_RIGHT, MOVE_RIGHT);
    add(VK_UP, MOVE_UP);
    add(VK_DOWN, MOVE_DOWN);
    add(VK_SPACE, RESET);
    add(KEY_F, ATTACK);
}

KeyMap::~KeyMap() {}
