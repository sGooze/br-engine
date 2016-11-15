// brd_texture.h - custom RGB8 2D texture class
//                 & custom Phong material class

#ifndef __BRD_TEXTURE

#include "brd_shared.h"
#include "brd_shader.h"
#include "brd_manager.h"

#define __BRD_TEXTURE

//----------------------------
// BRD_Texture2D - OpenGL texture loader & selector class
//----------------------------


class BRD_Texture2D{
private:
    //std::string tpath;
public:
    std::string tpath;
    // TODO: Destructor: free all GL resources
    GLuint tex;
    int width, height;

    BRD_Texture2D();
    BRD_Texture2D(const char* path);
    BRD_Texture2D(GLint w, GLint h, GLenum format, GLenum pixtype, const void* image, bool genmip = true);
    void use(){glBindTexture(GL_TEXTURE_2D, tex);}
    void clamp(GLenum S = GL_REPEAT, GLenum T = GL_REPEAT);
    void use(GLint tu){
        //std::cout << "using texID: " << tex << std::endl;
        glActiveTexture(GL_TEXTURE0 + tu);
        glBindTexture(GL_TEXTURE_2D, tex);
        glActiveTexture(GL_TEXTURE0);
    }
};

// TODO: Get rid of this in favor of texture manager
void BRD_Texture2D_InitBW();
BRD_Texture2D* black_tex();
BRD_Texture2D* white_tex();


//----------------------------
// BRD_TextureCube - OpenGL cubemap loader & selector class
//----------------------------

class BRD_TextureCube{
public:
    GLuint tex;
    int width, height;

    BRD_TextureCube(){};
    BRD_TextureCube(const char* path_prefix, const char* path_postfix);
    void use(GLint tu){
        //std::cout << "using texID: " << tex << std::endl;
        glActiveTexture(GL_TEXTURE0 + tu);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
        glActiveTexture(GL_TEXTURE0);
    }
};

// TODO: Get rid of this in favor of cubemap manager
void BRD_TextureCube_InitStd();
BRD_TextureCube* cubemap_std();

//----------------------------
// BRD_TextureList
//----------------------------

// First node should contain std error texture
/*
class BRD_TextureList{
private:
    BRD_TextureList* find_leaf(const char* path);  // Tries to find a leaf containing this texture, if it
                                                   // does not exist, returns NULL
public:
    BRD_Texture2D *tex;
    std::string tex_path;
    bool deleted;
    BRD_TextureList *left, *right;

    BRD_TextureList(const char* path);
    BRD_TextureList* load(const char* path);
    BRD_Texture2D* find(const char* path){
        // Returns pointer to the desired texture, if it isn't available - returns root texture
        BRD_TextureList *leaf = find_leaf(path);
        return (leaf != NULL) ? leaf->tex : tex;
    }
    void delete(const char* path);
};
*/
//----------------------------
// BRD_Material - OpenGL material, containing textures for
//--------------- Phong shader material
//----------------------------

#define BRD_MAT_DIFFUSE    0
#define BRD_MAT_SPECULAR   1
#define BRD_MAT_EMISSION   2
#define BRD_MAT_REFLECTION 3
#define BRD_MAT_CUBEMAP    4

class BRD_Material{
public:
    BRD_Texture2D* diffuse;
    BRD_Texture2D* specular;
    BRD_Texture2D* emission;
    BRD_TextureCube* cubemap;
    BRD_Texture2D* reflectivity;
    Shader* shader;             // Never NULL!!!
    GLfloat shininess;

    BRD_Material(){shininess = -1.0;};
    BRD_Material(GLfloat shin, const char* dif_path, const char* spec_path = NULL, const char* emi_path = NULL);
    BRD_Material(GLfloat shin, BRD_Texture2D* dif, BRD_Texture2D* spec = NULL, BRD_Texture2D* emi = NULL);
    void use(){
        diffuse->use(BRD_MAT_DIFFUSE);
        specular->use(BRD_MAT_SPECULAR);
        emission->use(BRD_MAT_EMISSION);
        if (shader != NULL)
            glUniform1f(glGetUniformLocation(shader->program, "mat.shininess"), shininess);
        else{

        }
    };
    void use(Shader& sh){
        diffuse->use(BRD_MAT_DIFFUSE);
        specular->use(BRD_MAT_SPECULAR);
        emission->use(BRD_MAT_EMISSION);
        glUniform1f(glGetUniformLocation(sh.program, "mat.shininess"), shininess);
    };
    void cubemap_add(BRD_TextureCube *sky){
        cubemap = sky;
    };
    void reflectivity_add(BRD_Texture2D *refl){
        reflectivity = refl;
    };
    void cubemap_use(Shader& sh){
        reflectivity->use(BRD_MAT_REFLECTION);
        cubemap->use(BRD_MAT_CUBEMAP);
    };
};

void BRD_SetShaderMatParams(Shader&);

//----------------------------
// BRD_Framebuffer
//----------------------------

class BRD_Framebuffer{
public:
    GLuint fbu;
    BRD_Texture2D* tex;
    GLuint rbf;
    GLuint width, height;

    BRD_Framebuffer(){};
    BRD_Framebuffer(GLint r_width, GLint r_height, BRD_Texture2D *cbuffer = NULL){
        glGenFramebuffers(1, &fbu);
        glBindFramebuffer(GL_FRAMEBUFFER, fbu);
            if (cbuffer == NULL){
                tex = new BRD_Texture2D(r_width, r_height, GL_RGB, GL_UNSIGNED_BYTE, NULL, false);
                std::cout << "New framebuffer texture generated! ID: " << tex->tex << std::endl;
            } else tex = cbuffer;
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->tex, 0);
            // .. renderbuffer
            glGenRenderbuffers(1, &rbf);
            glBindRenderbuffer(GL_RENDERBUFFER, rbf);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, r_width, r_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbf);
            // .. check for errors
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            else std::cout << "Framebuffer complete!\n";
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        width = r_width; height = r_height;
    };
    void use(){
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, fbu);
    }
    void use_default(){
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

// Texture Data Manager
bool TextureLoaderFunc(std::string& path, BRD_Texture2D&);
extern std::string dummyroot;
typedef binTree_Manager<BRD_Texture2D, TextureLoaderFunc, &dummyroot> BRD_Texture2D_Manager;

// Material Data Manager
bool MaterialLoaderFunc(std::string&, BRD_Material&);
extern std::string material_root;
typedef binTree_Manager<BRD_Material, MaterialLoaderFunc, &material_root> BRD_Material_Manager;

#endif // __BRD_TEXTURE
