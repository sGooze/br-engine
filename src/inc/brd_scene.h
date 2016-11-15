// brd_scene.h - scene management class
//          Loads and holds all scene related data.

#ifndef __BRD_SCENE
#include "brd_shared.h"
#include "brd_manager.h"

#include "brd_texture.h"
#include "brd_mesh.h"
#include "brd_object.h"
#include "brd_shader.h"

#define __BRD_SCENE

// Parser stuff - will be here for a while
typedef std::vector<std::string> tokenVec;
tokenVec StrToTokenVec(std::string);

//----------------------------
// BRD_Scene - contains level geometry and objects
//----------------------------

class BRD_Scene{
public:
    bool init;
    BRD_Texture2D_Manager TexMan;
    BRD_Material_Manager MatMan;
    BRD_Entity_Manager EntMan;
    BRD_Shader_Manager ShdMan;

    Shader *shader_current = NULL;

    BRD_Scene(){init = false;}
    BRD_Scene(const char* path);
};

//----------------------------
// BRD_Session - contains all data, related to the current game session
//------------ - (scene, cameras, settings, player parameters, etc.)
//----------------------------
/*
class BRD_Session{
public:

};
*/
#endif // __BRD_SCENE
