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

Renderer::GeometryLocation Renderer::addPoints(const std::vector<Point>& ps)
{
    uint batch_index = findPointsBatchIndexWithRoom(ps.size());
    std::vector<uint> element_indices = points[batch_index]->add(ps);
    return GeometryLocation(BatchLocation{.type = GL_POINTS, .batch_index = batch_index, .element_indices = element_indices});
}

Renderer::GeometryLocation Renderer::addLines(const std::vector<Line>& ls)
{
    uint batch_index = findLinesBatchIndexWithRoom(ls.size());
    std::vector<uint> element_indices = lines[batch_index]->add(ls);
    return GeometryLocation(BatchLocation{.type = GL_LINES, .batch_index = batch_index, .element_indices = element_indices});
}

Renderer::GeometryLocation Renderer::addTriangles(const std::vector<Triangle>& ts)
{
    uint batch_index = findTrianglesBatchIndexWithRoom(ts.size());
    std::vector<uint> element_indices = triangles[batch_index]->add(ts);
    return GeometryLocation(BatchLocation{.type = GL_TRIANGLES, .batch_index = batch_index, .element_indices = element_indices});
}

Renderer::GeometryLocation Renderer::addElements(const std::vector<Point>& ps, const std::vector<Line>& ls, const std::vector<Triangle>& ts)
{
    return addPoints(ps).add(addLines(ls)).add(addTriangles(ts));
}

Renderer::GeometryLocation Renderer::addPoint(const Point& p)
{
    return addPoints({p});
}

Renderer::GeometryLocation Renderer::addLine(const Line& l)
{
    return addLines({l});
}

Renderer::GeometryLocation Renderer::addTriangle(const Triangle& t)
{
    return addTriangles({t});
}

Renderer::GeometryLocation Renderer::addFrame(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    std::array<float, 3> col0 = {1, 0, 0};
    std::array<float, 3> col1 = {0, 1, 0};
    std::array<float, 3> col2 = {0, 0, 1};

    return addLines({
        Line(Point(p, col0), Point(p + v0, col0)),
        Line(Point(p, col1), Point(p + v1, col1)),
        Line(Point(p, col2), Point(p + v2, col2))
    });
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

    GeometryLocation where;
    where.add(BatchLocation{.type = GL_POINTS, .batch_index = points_batch_index, .element_indices = points[points_batch_index]->allElementIndices()});
    where.add(BatchLocation{.type = GL_LINES, .batch_index = lines_batch_index, .element_indices = lines[lines_batch_index]->allElementIndices()});
    where.add(BatchLocation{.type = GL_TRIANGLES, .batch_index = triangles_batch_index, .element_indices = triangles[triangles_batch_index]->allElementIndices()});
    return where;
}

Renderer::GeometryLocation Renderer::addConvexPolygon(const bool fill, const std::vector<glm::vec3>& points, const Color& color)
{
    const uint n = points.size();
    if (n < 3) return {};

    if (fill)
    {
        std::vector<Triangle> ts;
        for (uint i = 1; i < n-1; ++i)
        {
            Point p0(points[0], color);
            Point p1(points[i], color);
            Point p2(points[i+1], color);
            ts.emplace_back(p0, p1, p2);
        }
        return addTriangles(ts);
    }
    else
    {
        std::vector<Line> ls;
        for (uint i = 0; i < n; ++i)
        {
            Point p0(points[i], color);
            Point p1(points[(i+1)%n], color);
            ls.emplace_back(p0, p1);
        }
        return addLines(ls);
    }
}

Renderer::GeometryLocation Renderer::addSphere(const Vec3f& c, const float r, const Color& color, const uint precision)
{
    return addParametricSurface([&](float phi, float theta)
    {
        return c + Vec3f(
            r * sin(phi) * cos(theta),
            r * cos(phi),
            r * sin(phi) * sin(theta)
        );
    }, Vec2f(0,0), Vec2f(M_PI, 2.0*M_PI), color, precision);
}

Renderer::GeometryLocation Renderer::addTorus(const Vec3f& center, Vec3f axis, const float r, const float R, const Color& color, const uint precision)
{
    axis = glm::normalize(axis);
    const Vec3f e1 = glm::normalize(cross(abs(axis[2]) < 0.99f ? Vec3f(0,0,1) : Vec3f(1,0,0), axis));
    const Vec3f e2 = cross(axis, e1);

    return addParametricSurface([&](float u, float v)
    {
        return center
               + (R + r * cos(v)) * (cos(u) * e1 + sin(u) * e2)
               + r * sin(v) * axis;

    }, Vec2f(0,0), Vec2f(2.0*M_PI, 2.0*M_PI), color, precision);
}

Renderer::GeometryLocation Renderer::addTetrahedron(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Color& color)
{
    return addTriangles({
        Triangle(p0, p2, p1, color),
        Triangle(p3, p0, p1, color),
        Triangle(p3, p1, p2, color) ,
        Triangle(p0, p3, p2, color)
    });
}

Renderer::GeometryLocation Renderer::addParametricCurve(const std::function<Vec3f(float)>& f, const float min_t, const float max_t, const Color& color, const uint precision)
{
    std::vector<Line> ls;
    float t = min_t;
    Vec3f f0 = f(t);
    Vec3f f1;
    for (uint i = 0; i < precision; ++i)
    {
        t = min_t + (i+1) * (max_t - min_t) / precision;
        f1 = f(t);

        Point from(f0, color);
        Point to(f1, color);
        ls.emplace_back(from, to);

        f0 = f1;
    }
    return addLines(ls);
}

Renderer::GeometryLocation Renderer::addParametricSurface(const std::function<Vec3f(float,float)>& f, const Vec2f& min_st, const Vec2f& max_st, const Color& color, const uint precision)
{
    // Generate Vertex Positions
    std::vector<Point> points;
    float s, t;
    for (int i = 0; i <= precision; ++i)
    {
        s = min_st[0] + i * (max_st[0] - min_st[0]) / precision;
        for (int j = 0; j <= precision; ++j)
        {
            t = min_st[1] + j * (max_st[1] - min_st[1]) / precision;

            points.push_back(Point(f(s, t), color));
        }
    }

    // Generate Triangles
    std::vector<Triangle> ts;
    for (int i = 0; i < precision; ++i)
    {
        for (int j = 0; j < precision; ++j)
        {
            int row1 = i * (precision + 1);
            int row2 = (i + 1) * (precision + 1);

            ts.emplace_back(
                points[row1 + j + 0],
                points[row2 + j + 0],
                points[row2 + j + 1]
                );

            ts.emplace_back(
                points[row1 + j + 0],
                points[row2 + j + 1],
                points[row1 + j + 1]
                );
        }
    }
    return addTriangles(ts);
}

}
