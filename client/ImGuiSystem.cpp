#include "Entity.h"
#include "System.h"
#include "Component.h"
#include "ImGuiSystem.h"
#include "World.h"
#include "GraphicManager.h"
#include "Direct3D.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"


bool show_demo_window = true;
bool show_another_window = true;

ImGuiSystem::ImGuiSystem(GraphicManager* graphicManager, World* world) : m_graphicManager(graphicManager), m_world(world), is_updated(false)
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
    if (m_world != nullptr)
    {
        // Create box
        if (ImGui::Button("Create Box"))
        {
            m_world->addBox(box_x, box_y);
            box_x += 20;
            box_y -= 20;
        }
        ImGui::Spacing();

        // Delete any entity
        auto entities = m_world->getEntityIds();

        for (auto uuid : entities)
        {
            ImGui::PushID(uuid);
            ImGui::Text("Entity %d", uuid); ImGui::SameLine();
            if (ImGui::Button("Delete"))
            {
                Entity toDestroy = { uuid };
                m_world->destroyEntity(toDestroy);
            }
            ImGui::PopID();
        }

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
