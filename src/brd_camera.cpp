#include "inc/brd_camera.h"

BRD_Camera::BRD_Camera(glm::vec3 Pos,
                       glm::vec3 Front,
                       glm::vec3 Up){
    cameraFront = Front;
    cameraPos = Pos;
    cameraUp = Up;
    pitch = 0, yaw = -90.0f, roll = 0;
    sensitivity = 0.1f;
}

void BRD_Camera::rotate(GLfloat xshift, GLfloat yshift){
    yaw += xshift;
    pitch -= yshift;
    if(pitch > 89.9f)
      pitch =  89.9f;
    if(pitch < -89.9f)
      pitch = -89.9f;
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

void BRD_Camera::translate(glm::vec3 shift){
    cameraPos += shift;
}
