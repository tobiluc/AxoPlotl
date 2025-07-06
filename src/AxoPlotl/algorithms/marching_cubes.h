#pragma once

#include "AxoPlotl/commons/Mesh.h"
#include "AxoPlotl/utils/Typedefs.h"

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
    float x0 = -5, x1 = 5, y0 = -5, y1 = 5, z0 = -5, z1 = 5;

public:

    inline void setBounds(float x0, float x1, float y0, float y1, float z0, float z1) {
        this->x0 = x0;
        this->x1 = x1;
        this->y0 = y0;
        this->y1 = y1;
        this->z0 = z0;
        this->z1 = z1;
    }

    inline void setResolution(size_t nx, size_t ny, size_t nz) {
        this->nx = nx;
        this->ny = ny;
        this->nz = nz;
    }

    /// Generates a Triangle Mesh representing the surface f(x,y,z) = 0
    void generate(const std::function<float(Vec3f)>& f, TriangleMesh& mesh);

};

}
