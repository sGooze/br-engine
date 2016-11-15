// brd_imgui.h - imGui includes and interface functions

#ifndef __BRD_IMGUI
#include "../imgui/imgui.h"
#include "brd_shared.h"
#define __BRD_IMGUI

ImGuiIO& BRD_GUI_Init(SDL_Window *window);
void BRD_GUI_RenderDrawLists(ImDrawData* draw_data);
bool BRD_GUI_ProcessEvent(SDL_Event* event);
void BRD_GUI_Shutdown();
void BRD_GUI_NewFrame();

#endif // __BRD_IMGUI
