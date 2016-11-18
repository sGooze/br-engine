// brd_video.h

#ifndef __BRD_VIDEO
#define __BRD_VIDEO
#include "brd_shared.h"
#include "brd_shader.h"
#include "brd_texture.h"
#include "brd_mesh.h"
#include "brd_camera.h"

#include "brd_console.h"

bool    BRD_InitVideo();
void    BRD_InitGLParams();

extern SDL_Window* glWindow;       //The window we'll be rendering to
extern SDL_GLContext glContext;    //OpenGL context
extern char* glWinTitle;

extern cvar_int vid_screenwidth;
extern cvar_int vid_screenheight;

#endif // __BRD_VIDEO
