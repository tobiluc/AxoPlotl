#include "TetMeshRenderer.h"
#include "GLFW/glfw3.h"
#include "../commons/Camera.h"
#include "../utils/Globals.h"

namespace MV
{

void TetMeshRenderer::render()
{
    glm::mat4x4 model_matrix(1.0f);
    const auto& view_matrix = MV::CAMERA.getViewMatrix();
    const auto& projection_matrix = MV::CAMERA.getProjectionMatrix();

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
        shader.setBool("use_color_override", useColorOverride);
        shader.setVec3f("color_override", colorOverride);

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

        shader.setFloat("outline_width", outlineWidth);
        shader.setVec3f("outline_color", outlineColor);

        facesBatch.render(shader);
    }
    if (showEdges)
    {
        auto& shader = edgesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        shader.setVec2f("inverse_viewport_size", 1.f/VIEWPORT_SIZE[0], 1.f/VIEWPORT_SIZE[1]);

        shader.setFloat("line_width", lineWidth);

        edgesBatch.render(shader);
    }
    if (showVertices)
    {
        auto& shader = verticesShader;
        shader.use();

        shader.setFloat("time", glfwGetTime());

        shader.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);

        shader.setFloat("point_size", pointSize);


        verticesBatch.render(shader);
    }
}
}
