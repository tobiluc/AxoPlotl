#include "Renderer.h"
#include "../utils/Utils.h"
#include "../commons/Shader.h"

namespace MV
{

Renderer::Renderer()
{
}

void Renderer::render(const RenderMatrices& matrices)
{
    // Get Width and Height of Viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float width = viewport[2];
    const float height = viewport[3];

    //===========================
    // Points Primitives
    //===========================
    Shader::VERTICES_SHADER.use();
    Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", matrices.model_view_projection_matrix);
    Shader::VERTICES_SHADER.setFloat("point_size", settings.pointSize);

    for (uint i = 0; i < points.size(); ++i)
    {
        points[i]->render();
    }

    //===========================
    // Lines Primitives
    //===========================
    Shader::EDGES_SHADER.use();

    Shader::EDGES_SHADER.setMat4x4f("model_view_projection_matrix", matrices.model_view_projection_matrix);
    Shader::EDGES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);

    Shader::EDGES_SHADER.setFloat("line_width", settings.lineWidth);

    for (uint i = 0; i < lines.size(); ++i)
    {
        lines[i]->render();
    }

    //===========================
    // Triangles Primitives
    //===========================
    Shader::FACES_SHADER.use();

    Shader::FACES_SHADER.setMat4x4f("view_matrix", matrices.view_matrix);
    Shader::FACES_SHADER.setMat4x4f("model_view_projection_matrix", matrices.model_view_projection_matrix);
    Shader::FACES_SHADER.setMat3x3f("normal_matrix", matrices.normal_matrix);

    Shader::FACES_SHADER.setVec3f("light.position", Vec3f(0,0,0));
    Shader::FACES_SHADER.setVec3f("light.ambient", settings.light.ambient);
    Shader::FACES_SHADER.setVec3f("light.diffuse", settings.light.diffuse);
    Shader::FACES_SHADER.setVec3f("light.specular", settings.light.specular);

    Shader::FACES_OUTLINES_SHADER.use();

    Shader::FACES_OUTLINES_SHADER.setMat4x4f("model_view_projection_matrix", matrices.model_view_projection_matrix);
    Shader::FACES_OUTLINES_SHADER.setVec2f("inverse_viewport_size", 1.f/width, 1.f/height);
    Shader::FACES_OUTLINES_SHADER.setFloat("outline_width", settings.outlineWidth);
    Shader::FACES_OUTLINES_SHADER.setVec3f("outline_color", settings.outlineColor);

    for (uint i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->render();
    }

    glUseProgram(0);
}

void Renderer::renderPicking(const glm::mat4x4& mvp)
{
    for (uint i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->renderPicking(mvp, 3*i+2);
    }

    glUseProgram(0);
}

uint Renderer::findPointsBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < points.size(); ++i)
        if (points[i]->num_free_elements() >= n)
            return i;
    points.emplace_back(std::make_unique<PointsRenderBatch>(BATCH_SIZE));
    return points.size()-1;
}

uint Renderer::findLinesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < lines.size(); ++i)
        if (lines[i]->num_free_elements() >= n)
            return i;
    lines.emplace_back(std::make_unique<LinesRenderBatch>(BATCH_SIZE));
    return lines.size()-1;
}

uint Renderer::findTrianglesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < triangles.size(); ++i)
        if (triangles[i]->num_free_elements() >= n)
            return i;
    triangles.emplace_back(std::make_unique<TrianglesRenderBatch>(BATCH_SIZE));
    return triangles.size()-1;
}

uint Renderer::findEmptyPointsBatch()
{
    for (uint i = 0; i < points.size(); ++i)
        if (points[i]->num_elements() == 0)
            return i;
    points.emplace_back(std::make_unique<PointsRenderBatch>(BATCH_SIZE));
    return points.size()-1;
}

uint Renderer::findEmptyLinesBatch()
{
    for (uint i = 0; i < lines.size(); ++i)
        if (lines[i]->num_elements() == 0)
            return i;
    lines.emplace_back(std::make_unique<LinesRenderBatch>(BATCH_SIZE));
    return lines.size()-1;
}

uint Renderer::findEmptyTrianglesBatch()
{
    for (uint i = 0; i < triangles.size(); ++i)
        if (triangles[i]->num_elements() == 0)
            return i;
    triangles.emplace_back(std::make_unique<TrianglesRenderBatch>(BATCH_SIZE));
    return triangles.size()-1;
}

Renderer::PrimitivesLocations Renderer::addPoint(const Point& p)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findPointsBatchIndexWithRoom(1);

    // Add the point to the batch
    uint primitive_index = addPoint(batch_index, p);

    // Return the Location of the point
    return PrimitivesLocations{
        .type = GL_POINTS,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

Renderer::PrimitivesLocations Renderer::addLine(const Line& l)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findLinesBatchIndexWithRoom(1);

    // Add the line to the batch
    uint primitive_index = addLine(batch_index, l);

    // Return the Location of the line
    return PrimitivesLocations{
        .type = GL_LINES,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

Renderer::PrimitivesLocations Renderer::addTriangle(const Triangle& t)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findTrianglesBatchIndexWithRoom(1);

    // Add the Triangle to the batch
    uint primitive_index = addTriangle(batch_index, t);

    // Return the Location of the Triangle
    return PrimitivesLocations{
        .type = GL_TRIANGLES,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

Renderer::PrimitivesLocations Renderer::addFrame(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findLinesBatchIndexWithRoom(3);

    std::array<float, 3> col0 = {1, 0, 0};
    std::array<float, 3> col1 = {0, 1, 0};
    std::array<float, 3> col2 = {0, 0, 1};

    // Add Lines to the batch
    std::vector<uint> primitive_indices = {
        addLine(batch_index, Line(Point(p, col0), Point(p + v0, col0))),
        addLine(batch_index, Line(Point(p, col1), Point(p + v1, col1))),
        addLine(batch_index, Line(Point(p, col2), Point(p + v2, col2)))
    };

    return PrimitivesLocations{
        .type = GL_TRIANGLES,
        .batch_index = batch_index,
        .primitive_indices = primitive_indices
    };
}

Renderer::MeshLocation Renderer::addTetMesh(TetrahedralMesh& mesh)
{
    // Create a new set of batches for the mesh
    MeshLocation where{
        .points_batch_index = findEmptyPointsBatch(),
        .lines_batch_index = findEmptyLinesBatch(),
        .triangles_batch_index = findEmptyTrianglesBatch()
    };

    points[where.points_batch_index]->initFromMesh(mesh);
    lines[where.lines_batch_index]->initFromMesh(mesh);
    triangles[where.triangles_batch_index]->initFromMesh(mesh);

    return where;
}

Renderer::PrimitivesLocations Renderer::addAABB(const glm::vec3& min, glm::vec3& max)
{
    // // Find a Batch with Room or create a new one
    // uint batch_index = findLinesBatchIndexWithRoom(12);

    // std::array<float, 3> col = {0.5f, 1.f, 0.5f};

    // // Add Lines to the batch
    // std::vector<uint> primitive_indices = {
    //     addLine(batch_index, Line(Point(min, col), Point(min + , col0))),
    //     addLine(batch_index, Line(Point(p, col1), Point(p + v1, col1))),
    //     addLine(batch_index, Line(Point(p, col2), Point(p + v2, col2)))
    // };

    // return Location{
    //     .type = GL_TRIANGLES,
    //     .batch_index = batch_index,
    //     .primitive_indices = primitive_indices
    // };
}

}
