#ifndef MESHVIEWER_H
#define MESHVIEWER_H

#include "commons/PickingTexture.h"
#include "glad/glad.h"
#include "commons/Camera.h"
#include "rendering/MeshRenderer.h"
#include "rendering/PolygonRenderer.h"
#include "rendering/TrianglesRenderBatch.h"
#include "rendering/TetMeshRenderer.h"
#include "utils/Typedefs.h"

namespace MV
{

class MeshViewer
{
friend class ImGuiRenderer;
friend class TetMeshRenderer;

private:
class Mesh
{
public:
    Mesh() {}
    Mesh(const std::string& name, const std::shared_ptr<MeshRenderer>& mr) : name(name), renderer(mr)
    {}
    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;
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

    void deleteMesh(size_t i);

    void addTetMesh(const std::string& name, TetMeshRenderer& tmr);

    //void addHexMesh(HexMeshRenderer& hmr);

public:
    GLFWwindow* window;
    Camera camera;

private:

    Color clearColor;
    PickingTexture pickingTexture;
    PickingTexture::PixelData picked = {0,0,0};

    std::vector<Mesh> meshes;
    std::vector<PointsRenderBatch> points;
    std::vector<LinesRenderBatch> lines;
    std::vector<std::unique_ptr<TrianglesRenderBatch>> triangles;
    std::vector<std::unique_ptr<PolygonRenderer<4>>> quads;

    void init();

    void render();
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void checkOpenGLError();

}

#endif // MESHVIEWER_H
