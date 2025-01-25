#include "HexMeshRenderer.h"
#include "../commons/Camera.h"
#include "../utils/Globals.h"

namespace MV
{

void HexMeshRenderer::render(MeshViewer &mv)
{

    glm::mat4x4 model_matrix(1.0f);
    const auto& view_matrix = camera.getViewMatrix();
    const auto& projection_matrix = camera.getProjectionMatrix();

    const glm::mat4x4 model_view_matrix = view_matrix * model_matrix;
    const glm::mat4x4 model_view_projection_matrix = projection_matrix * model_view_matrix;
    const glm::mat3x3 normal_matrix = glm::transpose(glm::inverse(model_view_matrix));

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float width = viewport[2];
    float height = viewport[3];

    if (settings.showCells)
    {
        Shader::TET_CELLS_SHADER.use();

        Shader::TET_CELLS_SHADER.setFloat("cell_scale", settings.cellScale);

        Shader::TET_CELLS_SHADER.setMat4x4f("view_matrix", view_matrix);
        Shader::TET_CELLS_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::TET_CELLS_SHADER.setMat3x3f("normal_matrix", normal_matrix);

        Shader::TET_CELLS_SHADER.setVec3f("light.position", Vec3f(0,0,0));
        Shader::TET_CELLS_SHADER.setVec3f("light.ambient", settings.light.ambient);
        Shader::TET_CELLS_SHADER.setVec3f("light.diffuse", settings.light.diffuse);
        Shader::TET_CELLS_SHADER.setVec3f("light.specular", settings.light.specular);

        Shader::TET_CELLS_SHADER.setBool("use_color_override", settings.useColorOverride);
        Shader::TET_CELLS_SHADER.setVec3f("color_override", settings.colorOverride);

        hexCellsBatch.render();
    }
    if (settings.showEdges)
    {
        Shader::EDGES_SHADER.use();

        Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);
        Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);

        Shader::EDGES_SHADER.setFloat("line_width", settings.lineWidth);

        edgesBatch.render();
    }
    if (settings.showVertices)
    {
        Shader::VERTICES_SHADER.use();

        Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", model_view_projection_matrix);

        Shader::VERTICES_SHADER.setFloat("point_size", settings.pointSize);

        verticesBatch.render();
    }
}
}
