#pragma once

// Forward declarations
class System;
class GraphicManager;
class World;

class ImGuiSystem : public System {
public:
    ImGuiSystem(GraphicManager* graphicManager);
    void init();
    void update(double dt);
    void render();
    void entityWindow();

private:
    GraphicManager* m_graphicManager;
    bool is_updated;
};
