#include "TetMeshRenderer.h"
#include "GLFW/glfw3.h"

namespace MV
{

TetMeshRenderer::TetMeshRenderer(TetrahedralMesh& mesh) :
    mesh(mesh),
    cellScale(0.9f),
    tetCellsBatch(mesh),
    facesBatch(mesh),
    edgesBatch(mesh),
    edgesShader("../res/shaders/edges.vert", "../res/shaders/edges.frag"),
    facesShader("../res/shaders/faces.vert", "../res/shaders/faces_phong.frag"),
    tetCellsShader("../res/shaders/cells.vert", "../res/shaders/cells_phong.frag"),
    light({Vec3f(0.5,0.5,0.5), Vec3f(0.3,0.3,0.3), Vec3f(0.2,0.2,0.2)})
{
    //createEdgesBatch();
}

void TetMeshRenderer::render(const glm::mat4x4& v, const glm::mat4x4& p)
{
    if (showCells)
    {
        auto& shader = tetCellsShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setFloat("cell_scale", cellScale);

        shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        shader.setMat4x4f("view_matrix", v);
        shader.setMat4x4f("projection_matrix", p);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        tetCellsBatch.render(shader);
    }
    if (showFaces)
    {
        auto& shader = facesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        shader.setMat4x4f("view_matrix", v);
        shader.setMat4x4f("projection_matrix", p);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        facesBatch.render(shader);
    }
    if (showEdges)
    {
        auto& shader = edgesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setMat4x4f("model_view_projection_matrix", p * (v * Mat4x4f(1.0)));

        edgesBatch.render(shader);
    }
}
}
