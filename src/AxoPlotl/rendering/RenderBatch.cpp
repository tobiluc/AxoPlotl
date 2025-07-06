#include "RenderBatch.h"

namespace AxoPlotl::Rendering
{

// void Triangle::computeNormal()
// {
//     // Vectors from p0
//     std::array<float, 3> v0 = {vertices[1].position[0] - vertices[0].position[0],
//                                vertices[1].position[1] - vertices[0].position[1],
//                                vertices[1].position[2] - vertices[0].position[2]};

//     std::array<float, 3> v1 = {vertices[2].position[0] - vertices[0].position[0],
//                                vertices[2].position[1] - vertices[0].position[1],
//                                vertices[2].position[2] - vertices[0].position[2]};

//     // Compute cross product v0 × v1
//     normal[0] = v0[1] * v1[2] - v0[2] * v1[1];
//     normal[1] = v0[2] * v1[0] - v0[0] * v1[2];
//     normal[2] = v0[0] * v1[1] - v0[1] * v1[0];

//     // Normalize the normal vector
//     float length = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
//     if (length > 0.0f) {
//         normal[0] /= length;
//         normal[1] /= length;
//         normal[2] /= length;
//     }
// }

}
