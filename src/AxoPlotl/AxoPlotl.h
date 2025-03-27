#ifndef AXOPLOTL_H
#define AXOPLOTL_H

#include "AxoPlotl/geometry/AxPlInput.h"
#include "AxoPlotl/parsing/Scope.h"
#include "commons/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include "rendering/Renderer.h"

namespace AxPl
{

class MeshViewer
{
friend class ImGuiRenderer;

public:
    MeshViewer();

    ~MeshViewer();

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

public:
    GLFWwindow* window;
    Camera camera;

private:

    // Rendering
    Renderer renderer;

    Color clearColor;
    PickingTexture pickingTexture;
    PickingTexture::Pixel picked = {0,0,0};

    // Input Logic
    std::vector<AxPlInput> inputs;
    Scope rootScope;

    void init();

    void render();
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
