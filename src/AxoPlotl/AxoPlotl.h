#ifndef AXOPLOTL_H
#define AXOPLOTL_H

#include "AxoPlotl/Scene.h"
#include "glad/glad.h"
#include "rendering/MeshRenderer.h"

namespace AxoPlotl
{

class Runner
{

private:

    TestScene scene_;

    void init();

public:

    GLFWwindow* window;

    Runner();

    ~Runner();

    void run();

    inline float getViewportWidth()
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return viewport[2];
    }

    inline float getViewportHeight()
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        return viewport[3];
    }
};

inline void checkOpenGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << error << std::endl;
    }
}

inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

}

#endif // AXOPLOTL_H
