#ifndef MESHVIEWER_H
#define MESHVIEWER_H

#include "commons/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include "rendering/PolygonRenderer.h"
#include "rendering/Renderer.h"
#include "rendering/TrianglesRenderBatch.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Typedefs.h"

namespace MV
{

class MeshViewer
{
friend class ImGuiRenderer;
friend class TetMeshRenderer;

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

    Color clearColor;
    PickingTexture pickingTexture;
    PickingTexture::Pixel picked = {0,0,0};

    Renderer renderer;

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

#endif // MESHVIEWER_H
