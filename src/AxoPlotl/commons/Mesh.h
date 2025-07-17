#pragma once

#include "AxoPlotl/geometry/glm.h"
#include <iostream>
#include <vector>

namespace AxoPlotl
{

struct PolygonMesh
{
private:
    std::vector<Vec3f> vertices_;
    std::vector<std::vector<int>> polygons_;
    std::vector<std::vector<int>> v_neighbors_;
    std::vector<std::vector<int>> v_polygons_;

public:
    inline void clear() {
        vertices_.clear();
        polygons_.clear();
        v_polygons_.clear();
    }

    inline int addVertex(const Vec3f& p) {
        vertices_.push_back(p);
        v_polygons_.emplace_back();
        v_neighbors_.emplace_back();
        return vertices_.size()-1;
    }

    inline int addPolygon(const std::vector<int>& vs) {
        int n = vs.size();
        if (n < 3) return -1;
        int idx = polygons_.size();
        polygons_.push_back(vs);

        for (int j = 0; j < n; ++j) {
            v_neighbors_[vs[j]].push_back(vs[(j+n-1)%n]);
            v_neighbors_[vs[j]].push_back(vs[(j+1)%n]);
            v_polygons_[vs[j]].push_back(idx);
        }
        return idx;
    }

    inline const std::vector<int>& neighbors(int vidx) const {
        return v_neighbors_[vidx];
    }

    inline Vec3f polygonNormal(int idx) const {
        return glm::normalize(glm::cross(
            vertices_[polygons_[idx][2]] - vertices_[polygons_[idx][0]],
            vertices_[polygons_[idx][1]] - vertices_[polygons_[idx][0]]
            ));
    }

    inline Vec3f vertexNormal(int vidx) const {
        Vec3f n(0,0,0);
        for (int pidx : v_polygons_[vidx]) {
            n += polygonNormal(pidx);
        }
        return glm::normalize(n);
    }

    inline const Vec3f& vertex(int vidx) const {return vertices_[vidx];}

    inline const std::vector<int>& polygon(int pidx) const {return polygons_[pidx];}

    inline const std::vector<Vec3f>& vertices() const {return vertices_;}

    inline const std::vector<std::vector<int>>& polygons() const {return polygons_;}
};

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

// class Mesh
// {
// public:
//     struct Edge {
//         int v0, v1;
//     };

//     struct Face {
//         std::vector<int> e; // Edges
//     }

// private:
//     std::vector<Vec3f> vertices_;
//     std::vector<Edge> edges_;
//     std::vector<Face> faces_;

// public:
//     Mesh() {}

//     template<typename Vec3T>
//     int addVertex(const Vec3T& p) {
//         vertices_.emplace_back(p[0], p[1], p[2]);
//         return vertices_.size()-1;
//     }

//     int addEdge(int v0, int v1) {
//         edges_.emplace_back(v0, v1);
//         return edges_.size()-1;
//     }

//     int addFace(const std::vector<int>& edges) {
//         faces_.push_back(Cell{.dim=dim, .vertices=vertices});
//         return cells_.size()-1;
//     }

//     size_t numVertices() const {
//         return vertices_.size();
//     }

//     size_t numCells() const {
//         return cells_.size();
//     }

//     const std::vector<Vec3f> vertices() const {
//         return vertices_;
//     }

//     const std::vector<Cell> cells() const {
//         return cells_;
//     }

//     const Vec3f& vertex(int idx) const {
//         return vertices_[idx];
//     }

//     const Cell& cell(int idx) const {
//         return cells_[idx];
//     }

// };

}
