#pragma once

// Forward declarations
class System;

class TemplateSystem : public System {
public:
    TemplateSystem();
    void init();
    void update(double dt);
    void render();

};
