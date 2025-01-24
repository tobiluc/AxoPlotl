#ifndef MESHVIEWER_H
#define MESHVIEWER_H

#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/HexMeshRenderer.h"
#include "rendering/MeshRenderer.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Typedefs.h"

namespace MV
{

class MeshViewer
{
private:
class Mesh
{
public:
    Mesh() {}
    Mesh(const std::string& name, const std::shared_ptr<MeshRenderer>& mr) : name(name), renderer(mr)
    {}
    std::string name;
    std::shared_ptr<MeshRenderer> renderer;
};
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

    void addTetMesh(TetMeshRenderer& tmr);

    void addHexMesh(HexMeshRenderer& hmr);

public:
    GLFWwindow* window;
    Camera camera;

private:

    Color clearColor;

    std::vector<Mesh> meshes;

    void init();

    void render();
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void checkOpenGLError();

}

#endif // MESHVIEWER_H
