// brd_mesh.h - mesh class - for easy object handling and debugging

#ifndef __BRD_MESH
#define __BRD_MESH

#include "brd_shared.h"
#include "brd_manager.h"

#include <cstdio>

class BRD_Mesh{
private:
    // TODO: Destructor: free all GL resources
public:
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
    GLuint verts;
    GLuint verts_size;
    GLuint inds;
    GLfloat *vertices;
    GLuint  *indices;

    BRD_Mesh(){
        vbo = vao = ebo = verts = verts_size = inds = 0;
    }
    BRD_Mesh(GLfloat *mvertices, GLuint vert_amnt, GLuint param_amnt, GLuint *mindices, GLuint ind_amnt){
        // VBO allocation
        glGenBuffers(1, &vbo);
        // VAO allocation
        glGenVertexArrays(1, &vao);
        // EBO allocation
        if(ind_amnt > 0) glGenBuffers(1, &ebo);
        int param_size = (param_amnt == 3) ? 8 : 3 * param_amnt;
        vertices = new GLfloat[vert_amnt*param_size];
        verts = vert_amnt;
        verts_size = sizeof(GLfloat)*vert_amnt*param_size;
        int i;
        //std::cout << "PARAM SIZE " << param_size << std::endl;
        for (i = 0; i < vert_amnt*param_size; i++)
            {vertices[i] = mvertices[i];}
        inds = ind_amnt;
        if (inds > 0){
            indices = new GLuint[ind_amnt];
            for (i = 0; i < ind_amnt; i++)
                {indices[i] = mindices[i];}
        }

        // VAO configuration
        glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, this->verts_size, vertices, GL_STATIC_DRAW);
            if (ind_amnt > 0){
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*inds, indices, GL_STATIC_DRAW);
            }
            // Coordinate data
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)0);
            glEnableVertexAttribArray(0);
            if (param_amnt > 1){
                // Normal data
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)(3* sizeof(GLfloat)));
                glEnableVertexAttribArray(1);
                if (param_amnt > 2){
                    // Texture coordinate data
                    glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)(6 * sizeof(GLfloat)));
                    glEnableVertexAttribArray(2);
                }
            }
        glBindVertexArray(0);
    };

    BRD_Mesh(const char* path, GLuint param_amnt = 3){
        std::cout << "Loading from " << path << std::endl;
        std::ifstream meshFile;
        std::stringstream meshStream;
        std::string meshString;
        int i = 0;
        int param_size = (param_amnt == 3) ? 8 : 3 * param_amnt;
        meshFile.exceptions(std::ifstream::badbit);
        try{
            meshFile.open(path);
            meshStream << meshFile.rdbuf();
            while(std::getline(meshStream, meshString)){
                if ((meshString[0] == '\0')||(meshString[0] == '\n'))
                    continue;
                if (meshString[0] == '#'){
                    sscanf(meshString.c_str(), "#SIZE: V %i VT %i VN %i F %i",
                           &verts, &verts, &verts, &verts);
                    verts *= 3;
                    inds = verts;
                    vertices = new GLfloat[verts*param_size];
                    indices = new GLuint[inds];
                    for (i = 0; i < inds; i++)
                        {indices[i] = i;}
                    i = 0;
                }
                else{
                    if (verts == 0)
                        break;
                    sscanf(meshString.c_str(), "%f %f %f %f %f %f %f %f",
                           &vertices[i+0], &vertices[i+1], &vertices[i+2],
                           &vertices[i+3], &vertices[i+4], &vertices[i+5],
                           &vertices[i+6], &vertices[i+7]);
                    i+=8;
                }
            }
        }
        catch (std::ifstream::failure e){
            std::cout << "ERROR! Unable to read vlist file \"" << path << "\"!\n";
        }
        // VBO allocation
        glGenBuffers(1, &vbo);
        // VAO allocation
        glGenVertexArrays(1, &vao);
        // EBO allocation
        glGenBuffers(1, &ebo);
        verts_size = sizeof(GLfloat)*verts*param_size;
        // VAO configuration
        glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, this->verts_size, vertices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*inds, indices, GL_STATIC_DRAW);
            // Coordinate data
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)0);
            glEnableVertexAttribArray(0);
            if (param_amnt > 1){
                // Normal data
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)(3* sizeof(GLfloat)));
                glEnableVertexAttribArray(1);
                if (param_amnt > 2){
                    // Texture coordinate data
                    glVertexAttribPointer(2, 2, GL_FLOAT,GL_FALSE, sizeof(GLfloat) * param_size, (GLvoid*)(6 * sizeof(GLfloat)));
                    glEnableVertexAttribArray(2);
                }
            }
        std::cout << " LOADED " << verts << " VERTS\n";
        glBindVertexArray(0);
    };

    void DrawObject(bool nobind = false){
        if (nobind == true){
            glDrawElements(GL_TRIANGLES, this->inds, GL_UNSIGNED_INT, 0); return;
        }
        glBindVertexArray(vao);
        if (inds > 0)
            glDrawElements(GL_TRIANGLES, this->inds, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, this->verts);
        glBindVertexArray(0);
    }
};

// Mesh Data Manager

bool MeshLoaderFunc(std::string&, BRD_Mesh&);
extern std::string mesh_root;
typedef binTree_Manager<BRD_Mesh, MeshLoaderFunc, &mesh_root> BRD_Mesh_Manager;

#endif // __BRD_MESH
