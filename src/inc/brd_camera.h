// brd_camera.h - FPS camera class
#ifndef __BRD_CAMERA
#define __BRD_CAMERA

#include "brd_shared.h"

class BRD_Camera{
public:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    GLfloat pitch, yaw, roll;
    GLfloat sensitivity;

    BRD_Camera(glm::vec3 Pos = glm::vec3(0.0, 0.0, 0.0),
               glm::vec3 Front = glm::vec3(0.0, 0.0, -1.0),
               glm::vec3 Up = glm::vec3(0.0, 1.0, 0.0));
    void translate(glm::vec3 shift);
    void rotate(GLfloat xshift, GLfloat yshift);
    glm::mat4 lookat(){return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);}
};

#endif // __BRD_CAMERA
