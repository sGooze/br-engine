#include "inc/brd_init.h"
#include "inc/brd_video.h"
#include "inc/brd_object.h"

 bool flag_novid = false;
 cvar_int vid_screenheight = 600;
 cvar_int vid_screenwidth = 800;

 SDL_Window* glWindow;
 SDL_GLContext glContext;

static int argc;
static char** args;

static bool CheckFlag(const char* flag){
    for (int i = 1; i < argc; i++){
        if (strcmp(args[i], flag) == 0){
            DEBUG_COUT << " Flag set: " << flag;
            return true;
        }
    }
    return false;
}

bool BRD_Init(int m_argc, char* m_args[]){
    argc = m_argc;
    args = m_args;

    for (int i = 0; i < m_argc; i++)
        DEBUG_COUT << " Arg[" << i << "]: " << m_args[i] << std::endl;

    //Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		DEBUG_COUT << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		return false;
	}
	if (!CheckFlag("--novid")){
        if (!BRD_InitVideo())
            return false;
	}
    else flag_novid = true;

    atexit(BRD_Shutdown);

    BRD_InitClassTable();
	return true;
}

bool BRD_InitVideo(){
    //Use OpenGL 3.3 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    //Create window
    glWindow = SDL_CreateWindow(glWinTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vid_screenwidth, vid_screenheight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );
    if( glWindow == NULL ){
        DEBUG_COUT << "Window could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    //Create context
    glContext = SDL_GL_CreateContext( glWindow );
    if( glContext == NULL ){
        DEBUG_COUT << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    //Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK ){
        DEBUG_COUT << "Error initializing GLEW! " << glewGetErrorString(glewError) << std::endl;
        return false;
    }

    glViewport(0, 0, vid_screenwidth, vid_screenheight);
    BRD_Texture2D_InitBW();

    BRD_InitGLParams();
	ImGui_ImplSdlGL3_Init(glWindow);
	return true;
}

void BRD_InitGLParams(){
    // OpenGL parameters
    // TODO: Control each individual parameter via cvars.
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*glEnable(GL_SCISSOR_TEST);
	glScissor(250, 150, 300, 300);*/
}

void BRD_Shutdown(){
    if (flag_novid == false){
        //Destroy window
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow( glWindow );
        glWindow = NULL;
        ImGui_ImplSdlGL3_Shutdown();
    }

	//Quit SDL subsystems
	SDL_Quit();
}
