#ifndef AXOPLOTL_H
#define AXOPLOTL_H

#include "AxoPlotl/Scene.h"
#include "AxoPlotl/rendering/redraw_frames.h"
#include "glad/glad.h"
#include "rendering/MeshRenderer.h"

namespace AxoPlotl
{

inline void getViewportSize(GLint& width, GLint& height)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    width = viewport[2];
    height = viewport[3];
}

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
    Rendering::triggerRedraw();
}

}

#endif // AXOPLOTL_H
