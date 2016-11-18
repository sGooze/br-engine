// brd_shared.h - shared libraries and global constants

#ifndef __BRD_SHARED
#define __BRD_SHARED

// SDL, GL and GLEW includes
#define GLEW_STATIC
#include <SDL.h>
// TODO: Move GL-Specific includes to brd_video.h ?
#include <gl\glew.h>
#include <SDL_opengl.h>
#include <gl\glu.h>
#include <SOIL.h>

// GLM includes for vector & matrix operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ Library includes
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>

// ImGUI binding
#include "../imgui/imgui.h"
#include "imgui_impl_sdl_gl3.h"

//Screen dimension constants
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

// Scene defaults
//template<typename T, typename U> constexpr size_t offsetOfClassMember(U T::*member);
class  BRD_Scene;
extern BRD_Scene *cscene;

// Data type label. Used by the entity table to determine the necessary data offsets, and by the console
// do determine data type of the variables.
typedef enum{TYPE_NULL, TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_VEC3, TYPE_FILE, TYPE_MTL, TYPE_FUNC} paramType;

// Debug data output and input operations. Most of them will be removed or replaced with something more convenient
#define DEBUG_COUT std::cout
#define DEBUG_CIN  std::cin

#endif // __BRD_SHARED
