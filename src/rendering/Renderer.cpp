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

void Renderer::remove(const GeometryLocation& where)
{
    for (const auto& loc : where.locations)
    {
        switch (loc.type)
        {
        case GL_POINTS:
            points[loc.batch_index]->remove(loc.element_indices);
            break;
        case GL_LINES:
            lines[loc.batch_index]->remove(loc.element_indices);
            break;
        case GL_TRIANGLES:
            triangles[loc.batch_index]->remove(loc.element_indices);
            break;
        default:
            break;
        }
    }
}

uint Renderer::findPointsBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < points.size(); ++i)
        if (points[i]->num_free_elements() >= n)
            return i;
    points.emplace_back(std::make_unique<PointsRenderBatch>(std::max(n, BATCH_SIZE)));
    return points.size()-1;
}

uint Renderer::findLinesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < lines.size(); ++i)
        if (lines[i]->num_free_elements() >= n)
            return i;
    lines.emplace_back(std::make_unique<LinesRenderBatch>(std::max(n, BATCH_SIZE)));
    return lines.size()-1;
}

uint Renderer::findTrianglesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < triangles.size(); ++i)
        if (triangles[i]->num_free_elements() >= n)
            return i;
    triangles.emplace_back(std::make_unique<TrianglesRenderBatch>(std::max(n, BATCH_SIZE)));
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

Renderer::GeometryLocation Renderer::addPoint(const Point& p)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findPointsBatchIndexWithRoom(1);

    // Add the point to the batch
    uint primitive_index = addPoint(batch_index, p);

    // Return the Location of the point
    return GeometryLocation{{BatchLocation{
        .type = GL_POINTS,
        .batch_index = batch_index,
        .element_indices = {primitive_index}
    }}};
}

Renderer::GeometryLocation Renderer::addLine(const Line& l)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findLinesBatchIndexWithRoom(1);

    // Add the line to the batch
    uint primitive_index = addLine(batch_index, l);

    // Return the Location of the line
    return GeometryLocation{{BatchLocation{
        .type = GL_LINES,
        .batch_index = batch_index,
        .element_indices = {primitive_index}
    }}};
}

Renderer::GeometryLocation Renderer::addTriangle(const Triangle& t)
{
    // Find a Batch with Room or create a new one
    uint batch_index = findTrianglesBatchIndexWithRoom(1);

    // Add the Triangle to the batch
    uint primitive_index = addTriangle(batch_index, t);

    // Return the Location of the Triangle
    return GeometryLocation{{BatchLocation{
        .type = GL_TRIANGLES,
        .batch_index = batch_index,
        .element_indices = {primitive_index}
    }}};
}

Renderer::GeometryLocation Renderer::addFrame(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
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

    return GeometryLocation{{BatchLocation{
        .type = GL_TRIANGLES,
        .batch_index = batch_index,
        .element_indices = primitive_indices
    }}};
}

Renderer::GeometryLocation Renderer::addTetMesh(TetrahedralMesh& mesh)
{
    // Create a new set of batches for the mesh
    uint points_batch_index = findEmptyPointsBatch();
    uint lines_batch_index = findEmptyLinesBatch();
    uint triangles_batch_index = findEmptyTrianglesBatch();

    points[points_batch_index]->initFromMesh(mesh);
    lines[lines_batch_index]->initFromMesh(mesh);
    triangles[triangles_batch_index]->initFromMesh(mesh);

    return GeometryLocation{{
        BatchLocation{.type = GL_POINTS, .batch_index = points_batch_index, .element_indices = points[points_batch_index]->allElementIndices()},
        BatchLocation{.type = GL_LINES, .batch_index = lines_batch_index, .element_indices = lines[lines_batch_index]->allElementIndices()},
        BatchLocation{.type = GL_TRIANGLES, .batch_index = triangles_batch_index, .element_indices = triangles[triangles_batch_index]->allElementIndices()},
    }};
}

Renderer::GeometryLocation Renderer::addConvexPolygon(const bool fill, const std::vector<glm::vec3>& points, const Color& color)
{
    const uint n = points.size();
    if (n < 3) return {};

    if (fill)
    {
        uint batch_index = findTrianglesBatchIndexWithRoom(n - 2);

        // Add Triangles to the batch
        std::vector<uint> element_indices;
        for (uint i = 1; i < n-1; ++i)
        {
            Point p0(points[0], color);
            Point p1(points[i], color);
            Point p2(points[i+1], color);
            element_indices.push_back(triangles[batch_index]->add(Triangle(p0, p1, p2)));
        }

        return GeometryLocation{{
            BatchLocation{.type = GL_TRIANGLES, .batch_index = batch_index, .element_indices = element_indices},
        }};
    }
    else
    {
        uint batch_index = findLinesBatchIndexWithRoom(n);

        // Add Lines to the batch
        std::vector<uint> element_indices;
        for (uint i = 0; i < n; ++i)
        {
            Point p0(points[i], color);
            Point p1(points[(i+1)%n], color);
            element_indices.push_back(lines[batch_index]->add(Line(p0, p1)));
        }

        return GeometryLocation{{
         BatchLocation{.type = GL_LINES, .batch_index = batch_index, .element_indices = element_indices},
        }};
    }
}

Renderer::GeometryLocation Renderer::addExplicitCurve(const ExplicitCurve<float, float>& f)
{
    const uint n = 1000;
    const float x_min = 0;
    const float x_max = 100;

    uint batch_index = findLinesBatchIndexWithRoom(n-1);
    std::vector<uint> primitive_indices;

    for (uint i = 0; i < n-1; ++i)
    {
        float x0 = x_min + (i+0) * (x_max - x_min) / (n-1);
        float x1 = x_min + (i+1) * (x_max - x_min) / (n-1);
        float y0 = f.compute(x0);
        float y1 = f.compute(x1);

        Point from(glm::vec3(x0,y0,0), glm::vec3(1,0,0));
        Point to(glm::vec3(x1,y1,0), glm::vec3(0,0,1));
        primitive_indices.push_back(lines[batch_index]->add(Line(from, to)));
    }

    return GeometryLocation{
        .locations = {
            BatchLocation{
                .type = GL_LINES,
                .batch_index = batch_index,
                .element_indices = primitive_indices
            }
        }
    };
}

}
