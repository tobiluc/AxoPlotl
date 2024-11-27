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
        Shader::TET_CELLS_SHADER.use();

        Shader::TET_CELLS_SHADER.setFloat("cell_scale", cellScale);

        Shader::TET_CELLS_SHADER.setMat4x4f("view_matrix", view_matrix);
        Shader::TET_CELLS_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::TET_CELLS_SHADER.setMat3x3f("normal_matrix", normal_matrix);

        Shader::TET_CELLS_SHADER.setVec3f("light.position", Vec3f(0,0,0));
        Shader::TET_CELLS_SHADER.setVec3f("light.ambient", light.ambient);
        Shader::TET_CELLS_SHADER.setVec3f("light.diffuse", light.diffuse);
        Shader::TET_CELLS_SHADER.setVec3f("light.specular", light.specular);

        //shader.setFloat("outline_width", outlineWidth);
        //shader.setVec3f("outline_color", outlineColor);
        Shader::TET_CELLS_SHADER.setBool("use_color_override", useColorOverride);
        Shader::TET_CELLS_SHADER.setVec3f("color_override", colorOverride);

        tetCellsBatch.render();
    }
    if (showFaces)
    {
        Shader::FACES_SHADER.use();

        Shader::FACES_SHADER.setMat4x4f("view_matrix", view_matrix);
        Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::FACES_SHADER.setMat3x3f("normal_matrix", normal_matrix);

        Shader::FACES_SHADER.setVec3f("light.position", Vec3f(0,0,0));
        Shader::FACES_SHADER.setVec3f("light.ambient", light.ambient);
        Shader::FACES_SHADER.setVec3f("light.diffuse", light.diffuse);
        Shader::FACES_SHADER.setVec3f("light.specular", light.specular);

        Shader::FACES_OUTLINES_SHADER.use();

        Shader::FACES_OUTLINES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::FACES_OUTLINES_SHADER.setVec2f("inverse_viewport_size", 1.f/VIEWPORT_SIZE[0], 1.f/VIEWPORT_SIZE[1]);
        Shader::FACES_OUTLINES_SHADER.setFloat("outline_width", outlineWidth);
        Shader::FACES_OUTLINES_SHADER.setVec3f("outline_color", outlineColor);

        facesBatch.render();
    }
    if (showEdges)
    {
        Shader::EDGES_SHADER.use();

        Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/VIEWPORT_SIZE[0], 1.f/VIEWPORT_SIZE[1]);

        Shader::EDGES_SHADER.setFloat("line_width", lineWidth);

        edgesBatch.render();
    }
    if (showVertices)
    {
        Shader::VERTICES_SHADER.use();

        Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);

        Shader::VERTICES_SHADER.setFloat("point_size", pointSize);

        verticesBatch.render();
    }
}
}
