#include "inc/brd_menu.h"
#include "inc/brd_manager.h"

static void GUI_Send_SDL_QUIT(){
    /*SDL_Event event;
    event.quit.type = SDL_QUIT;
    event.quit.timestamp = 0;
    SDL_PushEvent(&event);*/
    exit(0);
}

void GUI_MainMenu(){
    float my = ImGui::GetIO().MousePos.y;
    if (my < 0.0)
        return;
    if (ImGui::BeginMainMenuBar()){
        if (ImGui::BeginMenu("File")){
            if (ImGui::MenuItem("Console", "", false, false)){}
            ImGui::Separator();
            if (ImGui::MenuItem("Quit application", "F10")){GUI_Send_SDL_QUIT();}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUI_FpsCounter(){
    bool gui_alwayson = true;
    int pos_y = 10;
    if (ImGui::GetIO().MousePos.y > -1)
        pos_y = 20;
    ImGui::SetNextWindowPos(ImVec2(10,pos_y));
    if (!ImGui::Begin("Debug info", &gui_alwayson, ImVec2(0,0), 0.3f,
        ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings
        |ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    if (ImGui::GetIO().MousePos.y > 0)
        ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    //ImGui::End();
}

/*void GUI_MemeMan(bool& meme){
    if (!ImGui::Begin("Debug info", &gui_alwayson, ImVec2(20,20), 0.3f,
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }
}*/

//void GUI_ManagerViewer(BRD_Texture2D_Manager& man){
//    bool gui_alwayson = true;
//    //ImGui::SetNextWindowPos(ImVec2(400, 20));
//    // ImGuiSetCond_FirstUseEver
//    if (!ImGui::Begin("Manager viewer", &gui_alwayson, ImVec2(400,150), 0.3f,
//        ImGuiWindowFlags_NoSavedSettings))
//    {
//        ImGui::End();
//        return;
//    }
//
//    ImGui::Columns(4, "table_manager");
//    ImGui::Separator();
//    ImGui::Text("ID"); ImGui::NextColumn();
//    ImGui::Text("Name"); ImGui::NextColumn();
//    ImGui::Text("Path"); ImGui::NextColumn();
//    ImGui::Text("Flags"); ImGui::NextColumn();
//    ImGui::Separator();
//    const char* names[3] = { "One", "Two", "Three" };
//    const char* paths[3] = { "/path/one", "/path/two", "/path/three" };
//    //static int selected = -1;
//    for (int i = 0; i < 3; i++)
//    {
//        char label[32];
//        sprintf(label, "%04d", i);
//        /*if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SpanAllColumns))
//            selected = i;*/
//        ImGui::NextColumn();
//        ImGui::Text(names[i]); ImGui::NextColumn();
//        ImGui::Text(paths[i]); ImGui::NextColumn();
//        ImGui::Text("...."); ImGui::NextColumn();
//    }
//    ImGui::Columns(1);
//    ImGui::Separator();
//
//    ImGui::End();
//}
