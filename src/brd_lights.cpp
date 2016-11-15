#include "inc/brd_object.h"
#include "inc/brd_scene.h"

//******************** Utility functions ********************//

glm::vec3 EulerToVec3(glm::vec3& angles){
    glm::vec3 front;
    front.x = cos(glm::radians(angles.x)) * cos(glm::radians(angles.y));
    front.y = sin(glm::radians(angles.x));
    front.z = cos(glm::radians(angles.x)) * sin(glm::radians(angles.y));
    return glm::normalize(front);
}

glm::vec3 Vec3ToEuler(glm::vec3& dir){
    GLfloat x = std::asin(dir.y);
    return glm::vec3(glm::degrees(x), glm::degrees(asin(dir.x/cos(x))), 0);
}

void BRD_Light::render_bulb(BRD_Model& mdl){
    BRD_Model bulb = mdl;
    bulb.angles = angles;
    bulb.position = position;
    bulb.direction = direction;
    bulb.render();
}

//******************** Constructors ********************//

BRD_Light::BRD_Light(glm::vec3 a, glm::vec3 d, glm::vec3 s){
    //light_sh = new Shader("shaders/light.vert", "shaders/light_lamp.frag");
    ambient = a;
    diffuse = d;
    specular = s;
}

BRD_LightDir::BRD_LightDir(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 dir): BRD_Light(a, d, s){
    angles = Vec3ToEuler(dir);
    direction = dir;
}

BRD_LightPoint::BRD_LightPoint(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos,
                   GLfloat lin, GLfloat quad, GLfloat cst) : BRD_Light(a, d, s){
    position = pos;
    constant = cst;
    linear = lin;
    quadratic = quad;
}

BRD_LightSpot::BRD_LightSpot(glm::vec3 a, glm::vec3 d, glm::vec3 s, glm::vec3 pos,
              glm::vec3 dir, GLfloat inCF, GLfloat outCF, GLfloat lin, GLfloat quad, GLfloat cst)
               : BRD_LightPoint(a, d, s, pos, lin, quad, cst){
    direction = dir;
    angles = Vec3ToEuler(direction);
    innerCutOff = std::cos(glm::radians(inCF));
    outerCutOff = std::cos(glm::radians(outCF));
}

//******************** Methods [::render] ********************//

void BRD_Light::ShowProperties(){
    bool gui_alwayson = true;
    ImGui::SetNextWindowSize(ImVec2(430,450), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Properties", &gui_alwayson, ImGuiWindowFlags_NoSavedSettings)){
        ImGui::End();
        return;
    }
    // TODO: Should we generate properties window contents based on table contents?
    ImGui::End();
}

void BRD_LightDir::render(){
    if ((cscene->shader_current->light_dir > cscene->shader_current->max_light_dir)||(enabled == false)||(visible == false))
        return;
    std::stringstream par_stream;
    par_stream << cscene->shader_current->pfx_dir << "[" << cscene->shader_current->light_dir++ << "]";
    std::string par = par_stream.str();
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".direction").c_str()), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".ambient").c_str()),  ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".diffuse").c_str()),  diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".specular").c_str()), specular.x, specular.y, specular.z);
}

void BRD_LightPoint::render(){
    if ((cscene->shader_current->light_point > cscene->shader_current->max_light_point)||(enabled == false)||(visible == false))
        return;
    std::stringstream par_stream;
    par_stream << cscene->shader_current->pfx_point << "[" << cscene->shader_current->light_point++ << "]";
    std::string par = par_stream.str();
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".ambient").c_str()),  ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".diffuse").c_str()),  diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".specular").c_str()), specular.x, specular.y, specular.z);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".quadratic").c_str()), quadratic);
}

void BRD_LightSpot::render(){
    if ((cscene->shader_current->light_spot > cscene->shader_current->max_light_spot)||(enabled == false)||(visible == false))
        return;
    std::stringstream par_stream;
    par_stream << cscene->shader_current->pfx_spot << "[" << cscene->shader_current->light_spot++ << "]";
    std::string par = par_stream.str();
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".position").c_str()), position.x, position.y, position.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".direction").c_str()), direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".ambient").c_str()),  ambient.x, ambient.y, ambient.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".diffuse").c_str()),  diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".specular").c_str()), specular.x, specular.y, specular.z);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".linear").c_str()), linear);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".quadratic").c_str()), quadratic);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".innerCutOff").c_str()), innerCutOff);
    glUniform1f(glGetUniformLocation(cscene->shader_current->program, std::string(par + ".outerCutOff").c_str()), outerCutOff);
}
