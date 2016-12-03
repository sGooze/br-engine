// brd_scene.h - scene management class
//          Loads and holds all scene related data.

#ifndef __BRD_SCENE
#include "brd_shared.h"
#include "brd_manager.h"

#include "brd_texture.h"
#include "brd_mesh.h"
#include "brd_object.h"
#include "brd_shader.h"
#include "brd_console.h"

#define __BRD_SCENE

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
    //CON_Record_Manager ConLocal;  // Local variables

    Shader *shader_current = NULL;

    BRD_Scene(){init = false;}
    BRD_Scene(const char* path, bool addin = false);
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
