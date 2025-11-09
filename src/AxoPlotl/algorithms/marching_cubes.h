#pragma once

#include "AxoPlotl/geometry/AABB.h"
#include <array>
#include <functional>
#include <glm/common.hpp>
#include <glm/glm.hpp>

namespace AxoPlotl::Algo
{

class MarchingCubes
{
public:
    struct Settings {
        glm::vec<3, uint32_t, glm::defaultp> resolution = {8,8,8};
        Geometry::AABB bounds = {-5,5,-5,5,-5,5};
        uint32_t max_adaptive_depth = 5;
    };

private:
    const static int edgeTable[256];
    const static int triangleTable[256][16];
    const static glm::vec3 cubeVertices[8];
    const static int cubeEdges[12][2];

    /// Generate Triangles on a single node
    static int generate(const std::function<float(glm::vec3)> &f, Geometry::AABB b, std::vector<glm::vec3>& points, std::vector<std::array<uint32_t,3>>& triangles);

public:

    /// Generates a Triangle Mesh representing the surface f(x,y,z) = 0
    static void generate(const std::function<float(glm::vec3)>& f,
                  std::vector<glm::vec3>& points, std::vector<std::array<uint32_t,3>>& triangles,
            const Settings& settings);

    /// Using an Octree
    static void generateAdaptive(const std::function<float(glm::vec3)>& f, std::vector<glm::vec3>& points, std::vector<std::array<uint32_t,3>>& triangles, const Settings& settings);

};

}
