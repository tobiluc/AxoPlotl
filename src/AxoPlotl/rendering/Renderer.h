#ifndef RENDERER_H
#define RENDERER_H

#include "GLBuffers.h"
#include "LinesRenderBatch.h"
#include "PointsRenderBatch.h"
#include "TrianglesRenderBatch.h"
#include <sys/types.h>

namespace AxoPlotl
{

class Renderer
{
public:

    // Describes a set of primitives within a batch
    struct BatchLocation
    {
        const GLenum type; // GL_POINTS, GL_LINES or GL_TRIANGLES
        const uint batch_index;
        const std::vector<uint> element_indices;
    };

    /*
     * Locations of Grouped Geometry.
     */
    struct GeometryLocation
    {
        std::vector<BatchLocation> locations;

        GeometryLocation() {}

        GeometryLocation(const BatchLocation& loc)
        {
            add(loc);
        }

        inline GeometryLocation& add(const BatchLocation& loc)
        {
            this->locations.push_back(loc);
            return *this;
        }

        inline GeometryLocation& add(const GeometryLocation& where)
        {
            for (const auto& loc : where.locations)
            {
                this->locations.push_back(loc);
            }
            return *this;
        }
    };

    struct RenderSettings
    {
        RenderSettings() :
            visible(true),
            showCells(true),
            showFaces(true),
            showEdges(true),
            showVertices(true),
            light({Color(0.7,0.7,0.7), Color(0.2,0.2,0.2), Color(0.1,0.1,0.1)}),
            cellScale(0.9f),
            outlineWidth(2.0f),
            lineWidth(2.0f),
            outlineColor(Color(0,0,0)),
            useColorOverride(false),
            colorOverride(Color(1,1,1))
        {}

        bool visible;
        bool showCells, showFaces, showEdges, showVertices;
        Light light;
        float cellScale;
        float outlineWidth;
        //float pointSize;
        float lineWidth;
        Color outlineColor;
        bool useColorOverride = false;
        Color colorOverride;
    };

    struct RenderMatrices
    {
        glm::mat4x4 model_matrix;
        glm::mat4x4 view_matrix;
        glm::mat4x4 projection_matrix;
        glm::mat4x4 model_view_matrix;
        glm::mat4x4 model_view_projection_matrix;
        glm::mat3x3 normal_matrix;

        RenderMatrices(const glm::mat4x4& m, const glm::mat4x4& v, const glm::mat4x4& p) :
            model_matrix(m),
            view_matrix(v),
            projection_matrix(p),
            model_view_matrix(v * m),
            model_view_projection_matrix(p * model_view_matrix),
            normal_matrix(glm::transpose(glm::inverse(model_view_matrix)))
        {}
    };

    RenderSettings settings;

private:
    std::vector<std::unique_ptr<PointsRenderBatch>> points;
    std::vector<std::unique_ptr<LinesRenderBatch>> lines;
    std::vector<std::unique_ptr<TrianglesRenderBatch>> triangles;

    // Max Number of Elements per Render Batch if not exceeded by the mesh size or requested number of free elements.
    constexpr static uint BATCH_SIZE = 512;

    // inline uint addPoint(uint batch_index, const Point& p)
    // {
    //     return points[batch_index]->add(p);
    // }

    // inline uint addLine(uint batch_index, const Line& l)
    // {
    //     return lines[batch_index]->add(l);
    // }

    // inline uint addTriangle(uint batch_index, const Triangle& t)
    // {
    //     return triangles[batch_index]->add(t);
    // }

    uint findPointsBatchIndexWithRoom(uint n);

    uint findLinesBatchIndexWithRoom(uint n);

    uint findTrianglesBatchIndexWithRoom(uint n);

    uint findEmptyPointsBatch();

    uint findEmptyLinesBatch();

    uint findEmptyTrianglesBatch();

public:
    Renderer();

    void render(const RenderMatrices& matrices);

    void renderPicking(const glm::mat4x4& mvp);

    void remove(const GeometryLocation& where);

    GeometryLocation addPoints(const std::vector<Point>& ps);

    GeometryLocation addLines(const std::vector<Line>& ls);

    GeometryLocation addTriangles(const std::vector<Triangle>& ts);

    GeometryLocation addElements(const std::vector<Point>& ps, const std::vector<Line>& ls, const std::vector<Triangle>& ts);

    GeometryLocation addPoint(const Point& p);

    GeometryLocation addLine(const Line& l);

    GeometryLocation addTriangle(const Triangle& t);

    // Shows three vectors originating at a common point p as a red, green and blue line
    GeometryLocation addFrame(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);

    GeometryLocation addTetMesh(TetrahedralMesh& mesh);

    GeometryLocation addConvexPolygon(const bool fill, const std::vector<glm::vec3>& points, const Color& color);

    GeometryLocation addSphere(const Vec3f& c, const float r, const Color& color, const uint precision = 16);

    /// Add a function f:R^3->R defined on the unit sphere visualized as the set
    /// (x*f(x), y*f(y), z*f(z)) where x^2+y^2+z^2=1
    GeometryLocation addSphericalHarmonic(const std::function<float(Vec3f)>& f, const float scale=1.0f, const uint precision = 32);

    GeometryLocation addTorus(const Vec3f& center, Vec3f axis, const float r, const float R, const Color& color, const uint precision = 16);

    GeometryLocation addTetrahedron(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Color& color);

    GeometryLocation addParametricCurve(const std::function<Vec3f(float)>& f, const float min_t, const float max_t, const Color& color, const uint precision = 32);

    GeometryLocation addParametricSurface(const std::function<Vec3f(float,float)>& f, const Vec2f& min_st, const Vec2f& max_st,
                                        const ColorFunction2f& color, const uint precision = 32);
};

}

#endif // RENDERER_H
