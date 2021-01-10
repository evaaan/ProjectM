#include "Entity.h"
#include "EntityHandle.h"
#include "System.h"
#include "Component.h"
#include "ImGuiSystem.h"
#include "World.h"
#include "GraphicManager.h"
#include "Direct2D.h"
#include "Direct3D.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


bool show_demo_window = true;
bool show_another_window = true;

void addBox(World* world, int x, int y)
{
    // Box
    EntityHandle solid_entity = world->createEntity();
    solid_entity.addComponent(Transform());
    solid_entity.addComponent(Dynamic());
    solid_entity.addComponent(Outline());
    auto solid_transform = solid_entity.getComponent<Transform>();
    solid_transform->width = 200;
    solid_transform->height = 200;
    auto solid_dynamic = solid_entity.getComponent<Dynamic>();
    solid_dynamic->width = solid_transform->width;
    solid_dynamic->height = solid_transform->height;
    solid_dynamic->pos.x = x;
    solid_dynamic->pos.y = y;
    solid_dynamic->type = BodyType::Ledge;
    auto solid_outline = solid_entity.getComponent<Outline>();
    solid_outline->color = Color::red;
    solid_outline->width = 1;
}

ImGuiSystem::ImGuiSystem(GraphicManager* graphicManager) : m_graphicManager(graphicManager), is_updated(false)
{
    // Add ComponentTypes the System acts on
    signature.addComponent<ImGuiSingleton>();
}

void ImGuiSystem::init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_graphicManager->appWindow);
    ImGui_ImplDX11_Init(m_graphicManager->m_direct3D->device.Get(), m_graphicManager->m_direct3D->deviceContext.Get());
}

/* System behaviors */
void ImGuiSystem::update(double dt)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    is_updated = true;

    entityWindow();
}

void ImGuiSystem::entityWindow()
{
    static int box_x = 500;
    static int box_y = 500;
    ImGui::Begin("Entities");
    // Create box
    if (ImGui::Button("Create Box"))
    {
        addBox(parentWorld.get(), box_x, box_y);
        box_x += 20;
        box_y -= 20;
    }
    ImGui::Spacing();

    // Delete any entity
    auto entities = parentWorld->getEntityIds();

    for (auto uuid : entities)
    {
        ImGui::PushID(uuid);
        ImGui::Text("Entity %d", uuid); ImGui::SameLine();
        if (ImGui::Button("Delete"))
        {
            Entity toDestroy = { uuid };
            parentWorld->destroyEntity(toDestroy);
        }
        ImGui::PopID();
    }

    ImGui::End();

}

/* System rendering */
void ImGuiSystem::render()
{
    // only render if we've displayed a frame
    if (is_updated)
    {
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
    is_updated = false;
}
