#include "inc/brd_texture.h"

/////////////////////////////
     // BRD_Texture2D //
/////////////////////////////

static BRD_Texture2D *BRD_Texture2D_Black, *BRD_Texture2D_White;

BRD_Texture2D::BRD_Texture2D(const char* path){
    int w, h;
    DEBUG_COUT << "Loading from " << path << std::endl;
    unsigned char* image = SOIL_load_image(path, &w, &h, 0, SOIL_LOAD_RGBA);
    if (image != '\0'){
        DEBUG_COUT << "Texture loaded! Size: " << w << "x" << h << std::endl;
        width = w; height = h;
        tpath = path;
        // TODO: Flip y-axis for loaded images

        glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D, tex);
            if (GLEW_EXT_texture_filter_anisotropic){
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0);
            }
            // Set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // Set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
            SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else{
        DEBUG_COUT << "ERROR! Unable to load texture!" << std::endl;
        tex = 0;
    }
}

void BRD_Texture2D::clamp(GLenum S, GLenum T){
    if ((width == 0)||(height == 0))
        return;
    glBindTexture(GL_TEXTURE_2D, tex);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, S);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, T);
    glBindTexture(GL_TEXTURE_2D, 0);
}

BRD_Texture2D::BRD_Texture2D(){
    width = height = 1;
    tpath = "void";
    unsigned char image[1] = {
        128
    };
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

BRD_Texture2D::BRD_Texture2D(GLint w, GLint h, GLenum format, GLenum pixtype, const void* image, bool genmip){
    width = w; height = h;
    tpath = "void";
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, pixtype, image);
        if (genmip == true) glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void BRD_Texture2D_InitBW(){
    BRD_Texture2D_Black = new BRD_Texture2D("textures/blank.png");
    BRD_Texture2D_White = new BRD_Texture2D("textures/blankw.png");
    BRD_TextureCube_InitStd();
}

BRD_Texture2D* black_tex() {return BRD_Texture2D_Black;};
BRD_Texture2D* white_tex() {return BRD_Texture2D_White;};

///////////////////////////////
     // BRD_TextureCube //
///////////////////////////////

static BRD_TextureCube *cubemap_std_data;

BRD_TextureCube::BRD_TextureCube(const char* path_prefix, const char* path_postfix){
    int w, h;
    DEBUG_COUT << "Loading cubemap from " << path_prefix << "X" << path_postfix << std::endl;
    // TODO: throw/catch for exceptions
    //const char* prefixes[6] = {"_right", "_left", "_top", "_bottom", "_back", "_front"};
    const char* prefixes[6] = {"rt", "lf", "up", "dn", "bk", "ft"};

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
        for (int i = 0; i < 6; i++){
            // std::string(path_prefix + prefixes[i] + path_postfix).c_str()
            std::stringstream path_stream;
            std::string path;
            path_stream << path_prefix << prefixes[i] << path_postfix;
            //path = path_stream.str(); path.c_str()
            DEBUG_COUT << "  [" << i << "]: " << path << "(";
            unsigned char* image = SOIL_load_image(path_stream.str().c_str(), &w, &h, 0, SOIL_LOAD_RGB);
            DEBUG_COUT << w << "x" << h << ")\n";
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            SOIL_free_image_data(image);
        }
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    DEBUG_COUT << "Texture loaded! \n";
}

void BRD_TextureCube_InitStd(){
    cubemap_std_data = new BRD_TextureCube("textures/skybox/2desert", ".bmp");
}

BRD_TextureCube* cubemap_std() {return cubemap_std_data;}

////////////////////////////
     // BRD_Material //
////////////////////////////

void BRD_SetShaderMatParams(Shader& sh){
    glUniform1i(glGetUniformLocation(sh.program, "mat.diffuse"),  BRD_MAT_DIFFUSE);
    glUniform1i(glGetUniformLocation(sh.program, "mat.specular"), BRD_MAT_SPECULAR);
    glUniform1i(glGetUniformLocation(sh.program, "mat.emission"), BRD_MAT_EMISSION);
    glUniform1i(glGetUniformLocation(sh.program, "mat.reflectivity"),  BRD_MAT_REFLECTION);
    glUniform1i(glGetUniformLocation(sh.program, "mat.cubemap"),  BRD_MAT_CUBEMAP);
}

BRD_Material::BRD_Material(GLfloat shin, const char* dif_path, const char* spec_path, const char* emi_path){
    // Search for materials in the current scene texture manager and save texture pointers
}

BRD_Material::BRD_Material(GLfloat shin, BRD_Texture2D* dif, BRD_Texture2D* spec, BRD_Texture2D* emi){
    shininess = shin;
    diffuse = dif;
    specular = spec;
    emission = emi;
    cubemap = cubemap_std();
    reflectivity = black_tex();
    shader = NULL;
}
