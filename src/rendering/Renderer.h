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
    struct Location
    {
        const GLenum type; // GL_POINTS, GL_LINES or GL_TRIANGLES
        const uint batch_index;
        const std::vector<uint> primitive_indices;
    };
private:
    std::vector<std::unique_ptr<PointsRenderBatch>> points;
    std::vector<std::unique_ptr<LinesRenderBatch>> lines;
    std::vector<std::unique_ptr<TrianglesRenderBatch>> triangles;

    uint addPoint(uint batch_index, const double pos[3], const float col[3]);

    uint addLine(uint batch_index, const double pos0[3], const double pos1[3], const float col[3]);

    uint addTriangle(uint batch_index, const double pos0[3], const double pos1[3], const double pos2[3], const float col[3]);

    uint findPointsBatchIndexWithRoom(uint n);

    uint findLinesBatchIndexWithRoom(uint n);

    uint findTrianglesBatchIndexWithRoom(uint n);

public:
    Renderer();

    void render();

    void remove(const Location& where);

    Location addPoint(const double pos[3], const float col[3]);

    Location addLine(const double pos0[3], const double pos1[3], const float col[3]);

    Location addTriangle(const double pos0[3], const double pos1[3], const double pos2[3], const float col[3]);
};

}

#endif // RENDERER_H
