#ifndef CAMERA_H
#define CAMERA_H

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AxoPlotl/IO/JSONFileAccessor.h"

namespace AxoPlotl
{

class BaseCamera
{
public:
    float sensitivity_ = 0.001f;
    float pan_speed_ = 2.0f;

    static constexpr glm::vec3 world_up = glm::vec3(0,1,0);
    static constexpr float near = 0.01f;
    static constexpr float far = 4096.0f;

    virtual glm::mat4 getViewMatrix() const = 0;
    virtual glm::mat4 getProjectionMatrix(float width_over_height) const = 0;

    inline glm::mat4 getProjectionMatrix() const {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        float width = viewport[2];
        float height = viewport[3];
        float aspect_ratio = width/height;
        return getProjectionMatrix(aspect_ratio);
    }

    virtual void update(GLFWwindow* window) = 0;

    virtual void reset(GLFWwindow* window) = 0;

    virtual void zoomToBox(const glm::vec3& min, const glm::vec3& max) = 0;
};

class PerspectiveCamera : public BaseCamera
{
private:
    glm::vec3 orbit_target_ = glm::vec3(0.0f);
    float orbit_distance_ = 10.0f;
    glm::vec3 position_;
    glm::vec3 up_;
    float yaw_ = 0; // Euler Angles in radians
    float pitch_ = 0;
    float fov_ = 0.25*M_PI; // field of view in radians

public:
    inline glm::mat4 getViewMatrix() const override {
        return lookAt(position_, orbit_target_, up_);
    }

    inline glm::mat4 getProjectionMatrix(float width_over_height) const override {
        return glm::perspective(fov_, width_over_height, near, far);
    }

    void update(GLFWwindow* window) override;

    void reset(GLFWwindow* window) override;

    void zoomToBox(const glm::vec3& min, const glm::vec3& max) override;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(PerspectiveCamera, position_, up_,
orbit_target_, orbit_distance_, yaw_, pitch_, fov_)
};

class OrthographicCamera : public BaseCamera
{
private:
    glm::vec3 position = glm::vec3(0,0,1);
    float height = 10;

public:
    inline glm::mat4 getViewMatrix() const override {
        return lookAt(position, position + glm::vec3(0,0,-1), world_up);
    }

    inline glm::mat4 getProjectionMatrix(float width_over_height) const override {
        const float ortho_height = height;
        const float ortho_width = width_over_height * ortho_height;
        return glm::ortho(-0.5f*ortho_width, 0.5f*ortho_width, -0.5f*ortho_height, 0.5f*ortho_height, near, far);
    }

    void update(GLFWwindow* window) override;

    void reset(GLFWwindow* window) override;

    void zoomToBox(const glm::vec3& min, const glm::vec3& max) override;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(OrthographicCamera, position, height)
};

struct CameraSet {
    PerspectiveCamera perspective;
    OrthographicCamera orthographic;

    bool use_ortho = false;

    inline BaseCamera* camera() {
        if (use_ortho) {return &orthographic;}
        return &perspective;
    }

    inline void reset(GLFWwindow* window) {
        perspective.reset(window);
        orthographic.reset(window);
    }

    inline void zoomToBox(const glm::vec3& min, const glm::vec3& max) {
        perspective.zoomToBox(min, max);
        orthographic.zoomToBox(min, max);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CameraSet, perspective, orthographic, use_ortho)
};

}

#endif
