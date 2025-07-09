#include "Renderer.h"
#include "../commons/Shader.h"
#include "AxoPlotl/geometry/glm.h"

namespace AxoPlotl::Rendering
{

Renderer::Renderer()
{
}

void Renderer::render(const RenderMatrices& matrices)
{
    if (!settings.visible) {return;}

    // Get Width and Height of Viewport
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float width = viewport[2];
    const float height = viewport[3];

    //===========================
    // Points Primitives
    //===========================
    Shader::VERTICES_SHADER.use();
    Shader::VERTICES_SHADER.setFloat("point_size", settings.pointSize);
    Shader::VERTICES_SHADER.setMat4x4f("model_view_projection_matrix", matrices.model_view_projection_matrix);

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

    glPolygonMode( GL_FRONT_AND_BACK, settings.wireframe? GL_LINE : GL_FILL );
    for (uint i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->render();
    }
    if (settings.wireframe) {glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );}

    glUseProgram(0);
}

void Renderer::renderPicking(const glm::mat4x4& mvp, uint object_index)
{
    if (!settings.visible) {return;}

    for (uint i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->renderPicking(mvp, object_index);
    }

    glUseProgram(0);
}

void Renderer::clear(const BatchIndices& where)
{
    for (const auto& loc : where)
    {
        switch (loc.first)
        {
        case GL_POINTS:
            points[loc.second]->clearVertexBuffer();
            break;
        case GL_LINES:
            lines[loc.second]->clearVertexBuffer();
            break;
        case GL_TRIANGLES:
            triangles[loc.second]->clearVertexBuffer();
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

void Renderer::addPoints(const std::vector<Point>& ps, BatchIndices& loc)
{
    uint batch_index = findPointsBatchIndexWithRoom(ps.size());
    points[batch_index]->add(ps);
    loc.emplace_back(GL_POINTS, batch_index);
}

void Renderer::addLines(const std::vector<Line>& ls, BatchIndices& loc)
{
    uint batch_index = findLinesBatchIndexWithRoom(ls.size());
    lines[batch_index]->add(ls);
    loc.emplace_back(GL_LINES, batch_index);
}

void Renderer::addTriangles(const std::vector<Triangle>& ts, BatchIndices& loc)
{
    uint batch_index = findTrianglesBatchIndexWithRoom(ts.size());
    triangles[batch_index]->add(ts);
    loc.emplace_back(GL_TRIANGLES, batch_index);
}

void Renderer::addElements(const std::vector<Point>& ps, const std::vector<Line>& ls, const std::vector<Triangle>& ts, BatchIndices& loc)
{
    addPoints(ps, loc);
    addLines(ls, loc);
    addTriangles(ts, loc);
}

void Renderer::addPoint(const Point& p, BatchIndices& loc)
{
    return addPoints({p}, loc);
}

void Renderer::addLine(const Line& l, BatchIndices& loc)
{
    addLines({l}, loc);
}

void Renderer::addTriangle(const Triangle& t, BatchIndices& loc)
{
    addTriangles({t}, loc);
}

void Renderer::addTetMesh(TetrahedralMesh& mesh, BatchIndices& loc)
{
    // Create a new set of batches for the mesh
    uint points_batch_index = findEmptyPointsBatch();
    uint lines_batch_index = findEmptyLinesBatch();
    uint triangles_batch_index = findEmptyTrianglesBatch();

    points[points_batch_index]->initFromMesh(mesh);
    lines[lines_batch_index]->initFromMesh(mesh);
    triangles[triangles_batch_index]->initFromMesh(mesh);

    loc.emplace_back(GL_POINTS, points_batch_index);
    loc.emplace_back(GL_LINES, lines_batch_index);
    loc.emplace_back(GL_TRIANGLES, triangles_batch_index);
}

void Renderer::addHexMesh(HexahedralMesh& mesh, BatchIndices& loc)
{
    //uint points_batch_index = findEmptyPointsBatch();
    //uint lines_batch_index = findEmptyLinesBatch();
    uint triangles_batch_index = findEmptyTrianglesBatch();

    //points[points_batch_index]->initFromMesh(mesh);
    //lines[lines_batch_index]->initFromMesh(mesh);
    triangles[triangles_batch_index]->initFromMesh(mesh);

    loc.emplace_back(GL_TRIANGLES, triangles_batch_index);
    //loc.add(BatchLocation{.type = GL_POINTS, .batch_index = points_batch_index, .element_indices = points[points_batch_index]->allElementIndices()});
    //loc.add(BatchLocation{.type = GL_LINES, .batch_index = lines_batch_index, .element_indices = lines[lines_batch_index]->allElementIndices()});
    //loc.add(BatchLocation{.type = GL_TRIANGLES, .batch_index = triangles_batch_index, .element_indices = triangles[triangles_batch_index]->allElementIndices()});
}

inline std::ostream& operator<<(std::ostream& os, const AxoPlotl::Vec3f& v) {
    return os << v.x << " " << v.y << " " << v.z;
}

void Renderer::addMesh(Mesh& mesh, BatchIndices& loc)
{
    // Add Points
    if (mesh.numVertices() > 0)
    {
        uint points_batch_index = findEmptyPointsBatch();
        points[points_batch_index]->regenerateBuffers(mesh.numVertices());
        for (int i = 0 ;i < mesh.numVertices(); ++i) {
            points[points_batch_index]->add(Point(mesh.vertex(i), Color::LIGHTGRAY));
        }
        loc.emplace_back(GL_POINTS, points_batch_index);
    }

    if (mesh.numCells() > 0)
    {
        uint triangles_batch_index = findEmptyTrianglesBatch();

        // Count number of required triangles
        size_t n_tris = 0;
        for (int i = 0; i < mesh.numCells(); ++i) {
            const auto& cell = mesh.cell(i);
            if (cell.dim == 2) {
                n_tris += cell.vertices.size()-2;
            }
        }


        // Add Triangles
        triangles[triangles_batch_index]->regenerateBuffers(n_tris);
        std::vector<Triangle> tris;
        tris.reserve(n_tris);

        for (int i = 0; i < mesh.numCells(); ++i) {
            const auto& cell = mesh.cell(i);

            if (cell.dim == 2) {
                for (int j = 1; j < cell.vertices.size()-1; ++j) {
                    int v0 = cell.vertices[0];
                    int v1 = cell.vertices[j];
                    int v2 = cell.vertices[j+1];

                    if (v0 < mesh.numVertices() && v1 < mesh.numVertices() && v2 < mesh.numVertices()) {

                        tris.emplace_back(
                            mesh.vertex(v0), mesh.vertex(v1), mesh.vertex(v2),
                            Color::WHITE
                            );
                    } else {
                        std::cerr << "Triangle " << v0 << ", " << v1 << ", " << v2 << std::endl;
                    }
                }
            }
        }
        triangles[triangles_batch_index]->add(tris);
        loc.emplace_back(GL_TRIANGLES, triangles_batch_index);

    }
}

// Renderer::GeometryLocation Renderer::addConvexPolygon(const bool fill, const std::vector<glm::vec3>& points, const Color& color)
// {
//     const uint n = points.size();
//     if (n < 3) return {};

//     if (fill)
//     {
//         std::vector<Triangle> ts;
//         for (uint i = 1; i < n-1; ++i)
//         {
//             Point p0(points[0], color);
//             Point p1(points[i], color);
//             Point p2(points[i+1], color);
//             ts.emplace_back(p0, p1, p2);
//         }
//         return addTriangles(ts);
//     }
//     else
//     {
//         std::vector<Line> ls;
//         for (uint i = 0; i < n; ++i)
//         {
//             Point p0(points[i], color);
//             Point p1(points[(i+1)%n], color);
//             ls.emplace_back(p0, p1);
//         }
//         return addLines(ls);
//     }
// }

// Renderer::GeometryLocation Renderer::addSphere(const Vec3f& c, const float r, const Color& color, const uint precision)
// {
//     return addParametricSurface([&](float phi, float theta)
//     {
//         return c + Vec3f(
//             r * sin(phi) * cos(theta),
//             r * cos(phi),
//             r * sin(phi) * sin(theta)
//         );
//     }, Vec2f(0,0), Vec2f(M_PI, 2.0*M_PI), color, precision);
// }

// Renderer::GeometryLocation Renderer::addSphericalHarmonic(const std::function<float(Vec3f)>& f,
//     const float scale, const uint precision)
// {
//     std::function<Vec3f(float,float)> func = [&](float phi, float theta)
//     {
//         Vec3f p(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
//         float r = f(p);
//         return Vec3f(
//            scale * r * p.x,
//            scale * r * p.y,
//            scale * r * p.z
//         );
//     };
//     std::function<Color(float,float)> color = [&](float phi, float theta)
//     {
//         Vec3f p(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
//         float r = f(p);
//         return Color::RED.mix(Color::BLUE, r);
//     };
//     return addParametricSurface(func, Vec2f(0,0), Vec2f(M_PI, 2.0*M_PI),
//         color, precision);
// }

// Renderer::GeometryLocation Renderer::addTorus(const Vec3f& center, Vec3f axis, const float r, const float R, const Color& color, const uint precision)
// {
//     axis = glm::normalize(axis);
//     const Vec3f e1 = glm::normalize(cross(abs(axis[2]) < 0.99f ? Vec3f(0,0,1) : Vec3f(1,0,0), axis));
//     const Vec3f e2 = cross(axis, e1);

//     return addParametricSurface([&](float u, float v)
//     {
//         return center
//                + (R + r * cos(v)) * (cos(u) * e1 + sin(u) * e2)
//                + r * sin(v) * axis;

//     }, Vec2f(0,0), Vec2f(2.0*M_PI, 2.0*M_PI), color, precision);
// }

// Renderer::GeometryLocation Renderer::addTetrahedron(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Color& color)
// {
//     return addTriangles({
//         Triangle(p0, p2, p1, color),
//         Triangle(p3, p0, p1, color),
//         Triangle(p3, p1, p2, color) ,
//         Triangle(p0, p3, p2, color)
//     });
// }

// Renderer::GeometryLocation Renderer::addParametricCurve(const std::function<Vec3f(float)>& f, const float min_t, const float max_t, const Color& color, const uint precision)
// {
//     std::vector<Line> ls;
//     float t = min_t;
//     Vec3f f0 = f(t);
//     Vec3f f1;
//     for (uint i = 0; i < precision; ++i)
//     {
//         t = min_t + (i+1) * (max_t - min_t) / precision;
//         f1 = f(t);

//         Point from(f0, color);
//         Point to(f1, color);
//         ls.emplace_back(from, to);

//         f0 = f1;
//     }
//     return addLines(ls);
// }

// Renderer::GeometryLocation Renderer::addParametricSurface(const std::function<Vec3f(float,float)>& f,
//     const Vec2f& min_st, const Vec2f& max_st, const ColorFunction2f& color, const uint precision)
// {
//     // Generate Vertex Positions
//     std::vector<Point> points;
//     float s, t;
//     for (int i = 0; i <= precision; ++i)
//     {
//         s = min_st[0] + i * (max_st[0] - min_st[0]) / precision;
//         for (int j = 0; j <= precision; ++j)
//         {
//             t = min_st[1] + j * (max_st[1] - min_st[1]) / precision;

//             points.push_back(Point(f(s, t), color(s, t)));
//         }
//     }

//     // Generate Triangles
//     std::vector<Triangle> ts;
//     for (int i = 0; i < precision; ++i)
//     {
//         for (int j = 0; j < precision; ++j)
//         {
//             int row1 = i * (precision + 1);
//             int row2 = (i + 1) * (precision + 1);

//             ts.emplace_back(
//                 points[row1 + j + 0],
//                 points[row2 + j + 0],
//                 points[row2 + j + 1]
//                 );

//             ts.emplace_back(
//                 points[row1 + j + 0],
//                 points[row2 + j + 1],
//                 points[row1 + j + 1]
//                 );
//         }
//     }
//     return addTriangles(ts);
// }

}
