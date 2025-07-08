#pragma once

#include "AxoPlotl/utils/Typedefs.h"
namespace AxoPlotl
{

struct TriangleMesh
{
    std::vector<Vec3f> vertices;
    std::vector<std::array<int,3>> triangles;

    template<typename Vec3T>
    int addVertex(const Vec3T& p) {
        vertices.emplace_back(p[0], p[1], p[2]);
        return vertices.size()-1;
    }

    int addTriangle(int v0, int v1, int v2) {
        triangles.push_back({v0, v1, v2});
        return triangles.size()-1;
    }
};

struct LineMesh
{
    std::vector<Vec3f> vertices;
    std::vector<std::array<int,2>> lines;

    template<typename Vec3T>
    int addVertex(const Vec3T& p) {
        vertices.emplace_back(p[0], p[1], p[2]);
        return vertices.size()-1;
    }

    int addLine(int v0, int v1) {
        lines.push_back({v0, v1});
        return lines.size()-1;
    }
};

typedef struct
{
    std::vector<Vec3f> vertices;
    std::vector<std::array<int,4>> tetrahedra;
} TetMesh;

typedef struct
{
    std::vector<Vec3f> vertices;
    std::vector<std::array<int,8>> hexahedra;
} HexMesh;

}
