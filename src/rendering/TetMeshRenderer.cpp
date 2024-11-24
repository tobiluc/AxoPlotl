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

void TetMeshRenderer::render(const glm::mat4x4& model_matrix, const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix)
{
    glm::mat4x4 model_view_matrix = view_matrix * model_matrix;
    glm::mat4x4 model_view_projection_matrix = projection_matrix * model_view_matrix;
    glm::mat3x3 normal_matrix = glm::transpose(glm::inverse(model_view_matrix));

    if (showCells)
    {
        auto& shader = tetCellsShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setFloat("cell_scale", cellScale);

        shader.setMat4x4f("view_matrix", view_matrix);
        shader.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        shader.setMat3x3f("normal_matrix", normal_matrix);

        shader.setVec3f("light.position", Vec3f(0,0,0));
        shader.setVec3f("light.ambient", light.ambient);
        shader.setVec3f("light.diffuse", light.diffuse);
        shader.setVec3f("light.specular", light.specular);

        shader.setFloat("outline_width", outlineWidth);
        shader.setVec3f("outline_color", outlineColor);

        tetCellsBatch.render(shader);
    }
    if (showFaces)
    {
        auto& shader = facesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setMat4x4f("view_matrix", view_matrix);
        shader.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        shader.setMat3x3f("normal_matrix", normal_matrix);

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

        shader.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);

        edgesBatch.render(shader);
    }
}
}
