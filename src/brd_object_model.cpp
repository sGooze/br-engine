#include "inc/brd_object.h"

BRD_Model::BRD_Model(){
    mesh = NULL;
    material = NULL;
    shader = NULL;
    position = angles = glm::vec3(0.0, 0.0, 0.0);
}

BRD_Model::BRD_Model(BRD_Mesh* msh, BRD_Material* mat, Shader* shd, glm::vec3 pos, glm::vec3 ang)
                    : BRD_Entity(pos, ang){
    mesh = msh;
    material = mat;
    shader = shd;
}

void BRD_Model::render(){
    // Matrices calculation
    // .. transformation matrices (including inverse matrices)
    glm::mat4 trans;
    trans = glm::translate(trans, position);
    trans = glm::rotate(trans, angles.x, glm::vec3(1.0, 0.0, 0.0));
    trans = glm::rotate(trans, angles.y, glm::vec3(0.0, 1.0, 0.0));
    trans = glm::rotate(trans, angles.z, glm::vec3(0.0, 0.0, 1.0));
    glm::mat4 transi = glm::inverse(trans);
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "transform"),
                            1, GL_FALSE, glm::value_ptr(trans));
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "transformi"),
                        1, GL_FALSE, glm::value_ptr(transi));
    material->use(*shader);
    material->cubemap_use(*shader);
    mesh->DrawObject();
}

void BRD_Model::ShowProperties(){
    //meme
}
