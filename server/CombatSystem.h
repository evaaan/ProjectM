#pragma once

// Forward declarations
class System;

class CombatSystem : public System {
public:
    CombatSystem();
    void init();
    void update(double dt);
    void render();
};
