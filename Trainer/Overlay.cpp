#include "Overlay.h"

Overlay::Overlay(Features::Collection* features, HWND& parentWindow)
    : m_Features(features), m_Window(parentWindow)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(parentWindow);
    ImGui_ImplOpenGL2_Init();
}

void Overlay::HookNavigation() {
    ImGuiIO& io = ImGui::GetIO();

    io.KeysDown[VK_UP] = GetAsyncKeyState(VK_NUMPAD8); //  | (GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_UP))
    io.KeysDown[VK_SPACE] = GetAsyncKeyState(VK_NUMPAD5);
    io.KeysDown[VK_DOWN] = GetAsyncKeyState(VK_NUMPAD2);
}

const char* Overlay::ResolveGroupName(Features::Feature::Group group) {
    switch (group)
    {
    case Features::Feature::Group::Core:
        return "Core";
    case Features::Feature::Group::BotOnly:
        return "Single player";
    default:
        return "Other";
    }
}

void Overlay::Render(Features::Feature* feature)
{
    switch (feature->widget)
    {
    case Features::Feature::Widget::Checkbox:
        ImGui::Checkbox(feature->name.c_str(), &feature->isEnabled);
        break;
    }
}

void Overlay::StartFrame()
{
    //Draw UI
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Overlay::Menu() {
    ImGui::Begin("Trainer");
    // UI Components
    // Iterate over features                          // Create a window called "Hello, world!" and append into it.
    ImGui::Text("FPS: %.1f (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
    // ImGui::GetForegroundDrawList()->AddCircle(ImVec2(100, 100), 100 * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);

    for (auto group : m_Features->Groups()) {

        if (ImGui::CollapsingHeader(ResolveGroupName(group), ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (auto feature : m_Features->InGroup(group))
            {
                Render(feature);
            }
        }
    }
    ImGui::End();
}

void Overlay::RenderFrame() {
    // Rendering
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

Overlay::~Overlay()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
