#pragma once

#include "AxoPlotl/commons/Mesh.h"
#include "AxoPlotl/geometry/Octree.h"
#include "AxoPlotl/geometry/glm.h"

namespace AxoPlotl::Algo
{

class MarchingCubes
{
private:
    const static int edgeTable[256];
    const static int triangleTable[256][16];
    const static Vec3f cubeVertices[8];
    const static int cubeEdges[12][2];

    size_t nx = 32, ny = 32, nz = 32; // number of vertices per dimension
    AABB bounds = {-5,5,-5,5,-5,5};

    /// Generate Triangles on a single node
    int generate(const std::function<float (Vec3f)> &f, AABB b, TriangleMesh &mesh);

public:

    inline void setBounds(AABB b) {
        this->bounds = b;
    }

    inline void setResolution(size_t nx, size_t ny, size_t nz) {
        this->nx = nx;
        this->ny = ny;
        this->nz = nz;
    }

    /// Generates a Triangle Mesh representing the surface f(x,y,z) = 0
    void generate(const std::function<float(Vec3f)>& f, TriangleMesh& mesh);

    void generateWithOctree(const std::function<float(Vec3f)>& f, TriangleMesh& mesh, Octree& tree, uint maxDepth = 5);

};

}
