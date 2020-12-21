#pragma once

// Forward declarations
class System;
class GraphicManager;

class OverlaySystem : public System {
public:
    OverlaySystem(GraphicManager* graphicManager);
    void init();
    void update(double dt);
    void render();

private:
    GraphicManager* m_graphicManager;
};
