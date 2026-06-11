#pragma once
#include <imgui.h>

IMGUI_IMPL_API bool ImGui_ImplBgfx_Init();
IMGUI_IMPL_API void ImGui_ImplBgfx_NewFrame();
IMGUI_IMPL_API void ImGui_ImplBgfx_RenderDrawLists(ImDrawData* draw_data);
IMGUI_IMPL_API void ImGui_ImplBgfx_Shutdown();
