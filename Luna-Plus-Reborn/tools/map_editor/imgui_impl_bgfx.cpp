#include <imgui.h>
#include <bgfx/bgfx.h>

static bool s_initialized = false;

bool ImGui_ImplBgfx_Init() {
    s_initialized = true;
    return true;
}

void ImGui_ImplBgfx_NewFrame() {
}

void ImGui_ImplBgfx_RenderDrawLists(ImDrawData* draw_data) {
    if (!s_initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    uint16_t width = static_cast<uint16_t>(io.DisplaySize.x);
    uint16_t height = static_cast<uint16_t>(io.DisplaySize.y);
    if (width == 0 || height == 0) return;
    bgfx::setViewName(10, "ImGui");
    bgfx::setViewClear(10, BGFX_CLEAR_NONE);
    bgfx::setViewRect(10, 0, 0, width, height);
    bgfx::touch(10);
}

void ImGui_ImplBgfx_Shutdown() {
    s_initialized = false;
}
