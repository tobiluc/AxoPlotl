#ifndef RENDERER_H
#define RENDERER_H

#include "GLBuffers.h"
#include "LinesRenderBatch.h"
#include "PointsRenderBatch.h"
#include "TrianglesRenderBatch.h"
#include <array>
#include <sys/types.h>
#include <unordered_set>

namespace MV
{

class Renderer
{
public:

    // Describes a set of primitives within a batch
    struct PrimitivesLocations
    {
        const GLenum type; // GL_POINTS, GL_LINES or GL_TRIANGLES
        const uint batch_index;
        const std::vector<uint> primitive_indices;
    };

    struct GeometryLocation
    {
        std::vector<PrimitivesLocations> locations;
    };

    struct MeshLocation
    {
        const uint points_batch_index;
        const uint lines_batch_index;
        const uint triangles_batch_index;
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
            pointSize(5.0f),
            outlineColor(Color(0,0,0)),
            useColorOverride(false),
            colorOverride(Color(1,1,1))
        {}

        bool visible;
        bool showCells, showFaces, showEdges, showVertices;
        Light light;
        float cellScale;
        float outlineWidth;
        float pointSize;
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

    // Max Number of Primitives per Render Batch
    constexpr static uint BATCH_SIZE = 8192;

    inline uint addPoint(uint batch_index, const Point& p)
    {
        return points[batch_index]->add(p);
    }

    inline uint addLine(uint batch_index, const Line& l)
    {
        return lines[batch_index]->add(l);
    }

    inline uint addTriangle(uint batch_index, const Triangle& t)
    {
        return triangles[batch_index]->add(t);
    }

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

    PrimitivesLocations addPoint(const Point& p);

    PrimitivesLocations addLine(const Line& l);

    PrimitivesLocations addTriangle(const Triangle& t);

    // Shows three vectors originating at a common point p as a red, green and blue line
    PrimitivesLocations addFrame(const glm::vec3& p, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);

    MeshLocation addTetMesh(TetrahedralMesh& mesh);

    PrimitivesLocations addAABB(const glm::vec3& min, glm::vec3& max);
};

}

#endif // RENDERER_H
