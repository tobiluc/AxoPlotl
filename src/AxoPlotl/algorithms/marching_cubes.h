#pragma once

#include "AxoPlotl/commons/Mesh.h"
#include "AxoPlotl/geometry/AABB.hpp"
#include "AxoPlotl/typedefs/typedefs_ToLoG.hpp"
#include "AxoPlotl/typedefs/typedefs_glm.hpp"

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
    int generate(const std::function<float (Vec3f)> &f, AABB b, SurfaceMesh &mesh);

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
    void generate(const std::function<float(Vec3f)>& f, SurfaceMesh& mesh);

    /// Using an Octree
    void generateAdaptive(const std::function<float(Vec3f)>& f, SurfaceMesh& mesh, uint maxDepth = 5);

};

}
