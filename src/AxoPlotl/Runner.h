#ifndef RUNNER_H
#define RUNNER_H

#include "AxoPlotl/Scene.h"
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

}

#endif // RUNNER_H
