#pragma once

// Forward declarations
class System;
class GraphicManager;

class AnimationSystem : public System {
public:
    AnimationSystem(GraphicManager* graphicManager);
    void init();
    void update(double dt);
    void render();

private:
    GraphicManager* m_graphicManager;
};
