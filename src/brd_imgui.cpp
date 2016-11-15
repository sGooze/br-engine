#include "inc/brd_imgui.h"
#include "inc/brd_shader.h"
#include <SDL_syswm.h>

// Data used for GUI rendering
static SDL_Window*  gui_window = NULL;
static Shader *gui_shader;
//static glm::mat4 gui_projection;

static GLuint gui_AttribProjection = 0, gui_AttribTexture = 0, gui_AttribPosition = 0, gui_FontTexture = 0,
              gui_AttribUV = 0, gui_AttribColor = 0, gui_VBO = 0, gui_VAO = 0, gui_EBO = 0;
static bool gui_MouseButtonsPressed[3] = {false};
static int gui_MouseWheel;
static double gui_time = 0.0f;

static void BRD_GUI_SetClipboard(const char* clipbrd){
    SDL_SetClipboardText(clipbrd);
}

static const char* BRD_GUI_GetClipboard(){
    return SDL_GetClipboardText();
}

void BRD_GUI_RenderDrawLists(ImDrawData* draw_data){
    // GUI is rendered into default framebuffer after everything else.
    ImGuiIO& io = ImGui::GetIO();
    glEnable(GL_SCISSOR_TEST);
    // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
    int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Framebuffer and texture params are expected to be set up before calling this function

    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);

    const float gui_projection[4][4] ={
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };

    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    gui_shader->use();
    glUniform1i(gui_AttribTexture, 0);
    glUniformMatrix4fv(gui_AttribProjection, 1, GL_FALSE, &gui_projection[0][0]);
    glBindVertexArray(gui_VAO);

    for (int n = 0; n < draw_data->CmdListsCount; n++){
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, gui_VBO);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert),
                     (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx),
                     (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++){
            if (pcmd->UserCallback){
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else{
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w),
                          (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                               sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }
    glBindVertexArray(0);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_CULL_FACE);
    //glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

bool BRD_GUI_ProcessEvent(SDL_Event* event){
    // Process event
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type){
    case SDL_MOUSEWHEEL:
        if (event->wheel.y > 0)
            gui_MouseWheel = 1;
        if (event->wheel.y < 0)
            gui_MouseWheel = -1;
        return true;
    case SDL_MOUSEBUTTONDOWN:
        for (int i = SDL_BUTTON_LEFT; i <= SDL_BUTTON_RIGHT; i++)
            if (event->button.button == i) gui_MouseButtonsPressed[i-SDL_BUTTON_LEFT] = true;
        return true;
    case SDL_TEXTINPUT:
        io.AddInputCharactersUTF8(event->text.text);
        return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
        io.KeysDown[key] = (event->type == SDL_KEYDOWN);
        io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
        io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
        io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
        return true;
    }
     return false;
}

void BRD_GUI_NewFrame(){
    ImGuiIO& io = ImGui::GetIO();
    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    SDL_GetWindowSize(gui_window, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Setup time step
    // TODO: Replace with global timer
    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = gui_time > 0.0 ? (float)(current_time - gui_time) : (float)(1.0f / 60.0f);
    gui_time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from SDL_PollEvent())
    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(gui_window) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);   // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    else
        io.MousePos = ImVec2(-1, -1);
    // If a mouse press event came, always pass it as "mouse held this frame",
    // so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[0] = gui_MouseButtonsPressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = gui_MouseButtonsPressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = gui_MouseButtonsPressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    gui_MouseButtonsPressed[0] = gui_MouseButtonsPressed[1] = gui_MouseButtonsPressed[2] = false;

    io.MouseWheel = gui_MouseWheel;
    gui_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    // Start the frame
    ImGui::NewFrame();
}

ImGuiIO& BRD_GUI_Init(SDL_Window *window){
    gui_window = window;

    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

    io.RenderDrawListsFn =  BRD_GUI_RenderDrawLists;
    io.SetClipboardTextFn = BRD_GUI_SetClipboard;
    io.GetClipboardTextFn = BRD_GUI_GetClipboard;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    // Setting up GUI shader and buffers
    gui_shader = new Shader("shaders/gui.vert", "shaders/gui.frag");
    // .. setting up unconventional VBO
    printf("  GUI_SHADER->PROGRAM = %i\n", (int)gui_shader->program);
    gui_AttribColor =       glGetUniformLocation(gui_shader->program, "color");
    gui_AttribPosition =    glGetUniformLocation(gui_shader->program, "position");
    gui_AttribProjection =  glGetUniformLocation(gui_shader->program, "ProjMtx");
    gui_AttribTexture =     glGetUniformLocation(gui_shader->program, "Texture");
    gui_AttribUV =          glGetUniformLocation(gui_shader->program, "UV");
    //gui_projection =        glm::ortho())

    glGenBuffers(1, &gui_VBO); glGenBuffers(1, &gui_EBO);
    glGenVertexArrays(1, &gui_VAO);
    glBindVertexArray(gui_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, gui_VBO);
        glEnableVertexAttribArray(gui_AttribColor);
        glEnableVertexAttribArray(gui_AttribUV);
        glEnableVertexAttribArray(gui_AttribPosition);
        #define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
            glVertexAttribPointer(gui_AttribPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
            glVertexAttribPointer(gui_AttribUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
            glVertexAttribPointer(gui_AttribColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
        #undef OFFSETOF
    glBindVertexArray(0);

    // .. setting up font texture
    unsigned char* pixels;
    int p_width, p_height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &p_width, &p_height);
    glGenTextures(1, &gui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, gui_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    io.Fonts->TexID = (void *)(intptr_t)gui_FontTexture;

    return io;
}

void BRD_GUI_Shutdown(){
    ImGui::Shutdown();
}
