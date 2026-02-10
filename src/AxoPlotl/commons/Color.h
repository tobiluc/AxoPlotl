#pragma once
#include "AxoPlotl/typedefs/typedefs_glm.hpp"

namespace AxoPlotl
{

Vec3f random_color();

inline Vec4f getColorOnSphere(float x, float y, float z) {
    float d = std::sqrt(x*x+y*y+z*z);
    return Vec4f(
        0.5 * (x/d + 1),
        0.5 * (y/d + 1),
        0.5 * (z/d + 1),
        1
        );
}

} // namespace AxoPlotl
