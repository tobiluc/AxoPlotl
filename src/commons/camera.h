#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace MV
{

class Camera {
public:
    enum Camera_Movement {FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

    // Camera Attributes
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    // Euler Angles
    float yaw;
    float pitch;

    // Camera Options
    float movement_speed;
    float sensitivity;
    float fov;

    Camera(glm::vec3 position, glm::vec3 forward);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    void processKeyboard(Camera_Movement dir, float delta_time);

    void processMouseScroll(float dy);

    void processMouseMovement(float dx, float dy);


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};
}

#endif
