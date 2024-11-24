#ifndef CAMERA_H
#define CAMERA_H

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ostream>
#include <vector>

namespace MV
{

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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
    glm::mat4 getProjectionMatrix();

    void processKeyboard(Camera_Movement dir, float delta_time);

    void processMouseScroll(float dy);

    void processMouseMovement(float dx, float dy);

    // static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    // {
    //     float aspect_ratio = (float)width / (float)height;
    //     float near = 0.1f;
    //     float far = 4096.f;
    //     glViewport(0, 0, width, height);
    // }


private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};
}

#endif
