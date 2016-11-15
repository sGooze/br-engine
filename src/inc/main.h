#ifndef __MAIN_H
#include "brd_shared.h"
#include "brd_shader.h"
#include "brd_texture.h"
#include "brd_mesh.h"
#include "brd_camera.h"

#include "brd_object.h"
#include "brd_manager.h"
#include "brd_scene.h"
#include "brd_menu.h"
#include "brd_console.h"

#define __MAIN_H

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes rendering program and clear color
bool initGL();

//Frees media and shuts down SDL
void close();

BRD_Mesh LoadTestObject();

//The window we'll be rendering to
SDL_Window* glWindow;

//OpenGL context
SDL_GLContext glContext;

#endif // __MAIN_H

