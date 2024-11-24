#include "TetMeshRenderer.h"
#include "GLFW/glfw3.h"

namespace MV
{

TetMeshRenderer::TetMeshRenderer(TetrahedralMesh& mesh) :
    mesh(mesh),
    cellScale(0.9f),
    outlineWidth(2),
    outlineColor({0,0,0}),
    tetCellsBatch(mesh),
    facesBatch(mesh),
    edgesBatch(mesh),
    edgesShader("../res/shaders/edges.vert", "../res/shaders/edges.frag"),
    facesShader("../res/shaders/faces.vert", "../res/shaders/faces_phong.frag"),
    tetCellsShader("../res/shaders/cells.vert", "../res/shaders/cells.geom", "../res/shaders/cells.frag"),
    light({Vec3f(0.5,0.5,0.5), Vec3f(0.3,0.3,0.3), Vec3f(0.2,0.2,0.2)})
{
}

void TetMeshRenderer::render(const glm::mat4x4& v, const glm::mat4x4& p)
{
    if (showCells)
    {
        tetCellsShader.use();

        tetCellsShader.setFloat("time", glfwGetTime());

        tetCellsShader.setFloat("cell_scale", cellScale);

        tetCellsShader.setMat4x4f("model_matrix", Mat4x4f(1.0));
        tetCellsShader.setMat4x4f("view_matrix", v);
        tetCellsShader.setMat4x4f("projection_matrix", p);

        tetCellsShader.setVec3f("light.position", Vec3f(0,0,0));
        tetCellsShader.setVec3f("light.ambient", light.ambient);
        tetCellsShader.setVec3f("light.diffuse", light.diffuse);
        tetCellsShader.setVec3f("light.specular", light.specular);

        tetCellsShader.setFloat("outline_width", outlineWidth);
        tetCellsShader.setVec3f("outline_color", outlineColor);

        tetCellsBatch.render(tetCellsShader);
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
