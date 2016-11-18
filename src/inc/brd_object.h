// brd_object.h - base object class

#ifndef __BRD_OBJECT_BASE

#include "brd_shared.h"
#include "brd_shader.h"
#include "brd_mesh.h"
#include "brd_texture.h"
#include "brd_manager.h"

#define __BRD_OBJECT_BASE

glm::vec3 EulerToVec3(glm::vec3& angles);
glm::vec3 Vec3ToEuler(glm::vec3& dir);

class BRD_EntityMaker;

//
// BRD_Entity - Base entity class.
//              All other entities are derivatives of this class.
//

class BRD_Entity{
public:
    glm::vec3 position;
    glm::vec3 angles;
    glm::vec3 direction;
    bool enabled = true;
    bool visible = true;
    BRD_EntityMaker *classmkr = NULL;
    // EntityList pointers:
    BRD_Entity *next_gen = NULL;
    BRD_Entity *next = NULL;

    BRD_Entity()
        {position = direction = angles = glm::vec3(0.0, 0.0, 0.0);}
    BRD_Entity(glm::vec3 pos, glm::vec3 ang)
        {position = pos; angles = ang; direction = EulerToVec3(angles);}
    virtual void translate(glm::vec3 trans)
        {position += trans;}
    virtual void rotate(glm::vec3 rot, bool replace = false){
        if (replace == false){
            angles += rot;
            if (angles.x >= 360.0) angles.x -= 360;
            if (angles.y >= 360.0) angles.y -= 360;
            if (angles.z >= 360.0) angles.z -= 360;
        } else angles = rot;
        direction = EulerToVec3(angles);
    }
    virtual void render(){};
    virtual void ShowProperties(){};
};

////////////////////////////
//// ENTITY TABLE       ////
////////////////////////////

class BRD_EntityParameter{
public:
    paramType type;
    size_t offset;
    std::string name;

    BRD_EntityParameter(){name = "!NULL", offset = 0, type = TYPE_NULL;}
    BRD_EntityParameter(paramType param_type, std::string param_name, size_t param_offset) : name(param_name){
        type = param_type;
        name = param_name;
        offset = param_offset;
    }
};

class BRD_EntityParameterTable{
public:
    std::vector<BRD_EntityParameter> table;
    //BRD_EntityParameterTable *parent = NULL; void SetParent
    //std::string classname = ""; void SetClassname
    //std::string classhelp = ""; void SetClasshelp

    BRD_EntityParameterTable(){};
    void add(paramType type, std::string name, size_t offset){
        table.push_back( BRD_EntityParameter(type, name, offset) );
    }
    BRD_EntityParameterTable(paramType type, std::string name, size_t offset){
        add(type, name, offset);
    }
    BRD_EntityParameterTable(BRD_EntityParameter& par){
        table.push_back(par);
    }

    BRD_EntityParameter* find(std::string& name){
        for (int i = 0; i < table.size(); i++){
            if (table[i].name == name)
                return &table[i];
        }
        std::cout << "BRD_EntityParameterTable::find - Unable to find parameter \"" << name << "\"\n";
        return NULL;
    }
};

typedef BRD_Entity* (*EntityMakerFunc)(void);

class BRD_EntityMaker{
public:
    std::string classname;
    EntityMakerFunc func;
    BRD_EntityParameterTable params;

    BRD_EntityMaker(){};
    BRD_EntityMaker(std::string name, EntityMakerFunc emf) : classname(name){
        func = emf;
    }
};

void BRD_InitClassTable();

////////////////////////////

class BRD_Model : public BRD_Entity{
public:
    BRD_Mesh       *mesh;
    BRD_Material   *material;
    // TODO: Replace with scene shader
    Shader         *shader;

    BRD_Model();
    BRD_Model(BRD_Mesh*, BRD_Material*, Shader*, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 ang = glm::vec3(0.0f));
    void render();
    void ShowProperties();
};

static BRD_Entity* mdl_static(void){
    return static_cast<BRD_Entity*>(new BRD_Model);
}

class BRD_Sprite : public BRD_Entity{
public:
    BRD_Material    *image;
    Shader          *shader;
    GLfloat         scalex, scaley;

    BRD_Shader() {};
};

///////////////////////
//// LIGHT CLASSES ////
///////////////////////

class BRD_Light : public BRD_Entity{
public:
    // TODO: Ambient light should be generated only by entirely separate entity (light_ambient)
    //       Specular color should be the same as the diffuse color
    //       ::use method should be replaced by ::render method, for consistency
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    //Shader *light_sh;

    //BRD_Light(){};
    BRD_Light(glm::vec3 a, glm::vec3 d, glm::vec3 s);
    void render_bulb(BRD_Model& mdl);
    void ShowProperties();
};

class BRD_LightDir : public BRD_Light{
public:
    BRD_LightDir(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 dir = glm::vec3(-0.2f, -1.0f, -0.3f));
    void render();
};

class BRD_LightPoint : public BRD_Light{
public:
    GLfloat constant, linear, quadratic;
    BRD_LightPoint(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos = glm::vec3(0),
                   GLfloat lin = 0.09, GLfloat quad = 0.032, GLfloat cst = 1.0);
    virtual void render();
};

class BRD_LightSpot : public BRD_LightPoint{
public:
    GLfloat innerCutOff, outerCutOff;
    BRD_LightSpot(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos = glm::vec3(0),
                  glm::vec3 dir = glm::vec3(0, -1.0f, 0), GLfloat inCF = 100, GLfloat outCF = 110,
                  GLfloat lin = 0.09, GLfloat quad = 0.032, GLfloat cst = 1.0);
    void render();
};

// EntityMaker & Entity Data Managers

extern std::string class_root;

bool EntMakerLoaderFunc(std::string&, BRD_EntityMaker&);
typedef binTree_Manager<BRD_EntityMaker, EntMakerLoaderFunc, &class_root> BRD_EntityMaker_Manager;
extern BRD_EntityMaker_Manager ent_class_table;

bool EntityLoaderFunction(std::string&, BRD_Entity*&);
typedef binTree_Manager<BRD_Entity*, EntityLoaderFunction, &class_root> BRD_Entity_Manager;

#endif // __BRD_OBJECT_BASE
