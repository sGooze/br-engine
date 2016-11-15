// brd_portal.h - portal-based VIS and object rendering subsystem
#ifndef __BRD_PORTAL
#include "brd_shared.h"
#define __BRD_PORTAL

class PR_Face4{
public:
    GLuint verts_amnt;
    GLuint verts[4];
    bool isPortal;

    PR_Face3(GLuint v1, GLuint v2, GLuint v3, GLuint v4){
        verts[0] = v1; verts[1] = v2; verts[2] = v3; verts[3] = v4;
        isPortal = false;
    };
};

extern long sectors;

class PR_Sector{
public:
    GLuint id;
    PR_Face4 *planes;

    PR_Sector(){id = sectors++;}
    void add_plane(GLuint v1, GLuint v2, GLuint v3, GLuint v4, bool portal = false);
};
/*
class PR_Scene{
public:
    //PR_Sector
    GLuint lights_amnt;
    BRD_Light *lights[MAX_LIGHTS];

    PR_Scene(){};
    void AddLight(BRD_Light);
};
*/
#endif // __BRD_PORTAL
