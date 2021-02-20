#pragma once
#include <bitset>
#include <windows.h>
#include <unordered_map>

#define NUM_KEYBINDINGS 64

enum Action
{
    DEFAULT = 0,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_UP,
    MOVE_DOWN,
    RESET,
    ATTACK
};

/* Wrapper around two hash tables to map Windows Virtual-Key Codes to Action enums.
Virtual-Key Code list: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes */
class KeyMap
{
public:
    KeyMap();
    ~KeyMap();
    void inline add(WPARAM keyCode, Action action) { keyCodeMap[keyCode] = action; actionMap[action] = keyCode; }
    void inline removeKeyCode(WPARAM keyCode) { keyCodeMap.erase(keyCode); }
    void inline removeAction(Action action) { actionMap.erase(action); }
    WPARAM inline getKeyCode(Action action) { return actionMap[action]; }
    Action inline getAction(WPARAM keyCode) { return keyCodeMap[keyCode]; }

    std::unordered_map<WPARAM, Action> keyCodeMap; // Map key codes to actions
    std::unordered_map<Action, WPARAM> actionMap;  // Map actions to key codes
};

/* Helper function to see if a key is pressed */
bool inline keyDown(std::bitset<NUM_KEYBINDINGS> state, Action action)
{
    return state.test(action);
}
