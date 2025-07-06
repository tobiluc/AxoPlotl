#pragma once

#include "AxoPlotl/utils/Typedefs.h"
namespace AxoPlotl
{

typedef struct
{
    std::vector<Vec3f> vertices;
    std::vector<Vec3i> triangles;

} TriangleMesh;

typedef struct
{
    std::vector<Vec3f> vertices;
    std::vector<Vec2i> lines;

} LineMesh;

}
