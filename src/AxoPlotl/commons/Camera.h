#ifndef CAMERA_H
#define CAMERA_H

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <ostream>
#include <vector>

namespace AxoPlotl
{

class Camera {
public:
    enum CameraMovement {FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN};

    // Camera Options
    bool isOrthographic = false;

    Camera(glm::vec3 position, glm::vec3 forward);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getProjectionMatrix(float width_over_height);

    void update(GLFWwindow* window);

    void processKeyboard(GLFWwindow* window);

    void processKeyboard(CameraMovement dir);

    void processMouseScroll(float dy);

    void processMouseMovement(float dx, float dy);

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    static constexpr glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr float sensitivity = 0.001f; // mouse movement and scroll
    static constexpr float movement_speed = 40.0f;

    struct PerspectiveProjection
    {
        // Camera Attributes
        glm::vec3 position;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;

        // Euler Angles in radians
        float yaw;
        float pitch;

        float fov; // field of view in radians

    } perspective;

    struct OrthographicProjection
    {
        glm::vec3 position;
        float height;
    } orthographic;
};
}

#endif
