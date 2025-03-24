#include "Renderer.h"

namespace MV
{

Renderer::Renderer()
{
}

void Renderer::render()
{
    for (uint i = 0; i < points.size(); ++i)
    {
        points[i]->render();
    }

    for (uint i = 0; i < lines.size(); ++i)
    {
        lines[i]->render();
    }

    for (uint i = 0; i < triangles.size(); ++i)
    {
        triangles[i]->render();
    }
}

void Renderer::remove(const Location& where)
{
    switch (where.type)
    {
    case GL_POINTS: points[where.batch_index]->remove(where.primitive_indices); break;
    case GL_LINES: lines[where.batch_index]->remove(where.primitive_indices); break;
    case GL_TRIANGLES: triangles[where.batch_index]->remove(where.primitive_indices); break;
    default: break;
    }
}

Renderer::Location Renderer::addPoint(const double pos[3], const float col[3])
{
    // Find a Batch with Room or create a new one
    uint batch_index = findPointsBatchIndexWithRoom(1);

    // Add the point to the batch
    uint primitive_index = addPoint(batch_index, pos, col);

    // Return the Location of the point
    return Location{
        .type = GL_POINTS,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

Renderer::Location Renderer::addLine(const double pos0[3], const double pos1[3], const float col[3])
{
    // Find a Batch with Room or create a new one
    uint batch_index = findLinesBatchIndexWithRoom(1);

    // Add the line to the batch
    uint primitive_index = addLine(batch_index, pos0, pos1, col);

    // Return the Location of the line
    return Location{
        .type = GL_LINES,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

Renderer::Location Renderer::addTriangle(const double pos0[3], const double pos1[3], const double pos2[3], const float col[3])
{
    // Find a Batch with Room or create a new one
    uint batch_index = findTrianglesBatchIndexWithRoom(1);

    // Add the Triangle to the batch
    uint primitive_index = addTriangle(batch_index, pos0, pos1, pos2, col);

    // Return the Location of the Triangle
    return Location{
        .type = GL_TRIANGLES,
        .batch_index = batch_index,
        .primitive_indices = {primitive_index}
    };
}

uint Renderer::addPoint(uint batch_index, const double pos[3], const float col[3])
{
    return points[batch_index]->add(Point(pos, col));
}

uint Renderer::addLine(uint batch_index, const double pos0[3], const double pos1[3], const float col[3])
{
    return lines[batch_index]->add(Line(Point(pos0, col), Point(pos1, col)));
}

uint Renderer::addTriangle(uint batch_index, const double pos0[3], const double pos1[3], const double pos2[3], const float col[3])
{
    return triangles[batch_index]->add(Triangle(
        Point(pos0, col),
        Point(pos1, col),
        Point(pos2, col)
    ));
}

uint Renderer::findPointsBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < points.size(); ++i)
        if (points[i]->numFree() >= n)
            return i;
    points.emplace_back(std::make_unique<PointsRenderBatch>(10000));
    return points.size()-1;
}

uint Renderer::findLinesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < lines.size(); ++i)
        if (lines[i]->numFree() >= n)
            return i;
    lines.emplace_back(std::make_unique<LinesRenderBatch>(10000));
    return lines.size()-1;
}

uint Renderer::findTrianglesBatchIndexWithRoom(uint n)
{
    for (uint i = 0; i < triangles.size(); ++i)
        if (triangles[i]->numFree() >= n)
            return i;
    triangles.emplace_back(std::make_unique<TrianglesRenderBatch>(10000));
    return triangles.size()-1;
}

}
