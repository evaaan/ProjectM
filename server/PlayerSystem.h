#pragma once

// Forward declarations
class System;

class PlayerSystem : public System {
public:
    PlayerSystem();
    void init();
    void update(double dt);
    void render();

private:
    ComponentHandle<WorldDeltaSingleton> worldDelta;
    ComponentHandle<KeyStateSingleton> keys;
    void updatePlayerState();
    void updatePlayerAnimation();

};
