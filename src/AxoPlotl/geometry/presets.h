#pragma once

#include "../utils/Utils.h"

namespace AxoPlotl
{

typedef struct
{
    std::function<Vec3f(float,float)> f;
    float uMin, uMax, vMin, vMax;

} ExplicitSurfaceFunction;

class ExplictSurfaceFunctionBuilder
{
public:
    static ExplicitSurfaceFunction sphere(const Vec3f& center = Vec3f(0,0,0), float radius = 1.0f);

    static ExplicitSurfaceFunction torus(const Vec3f& center = Vec3f(0,0,0), Vec3f axis = Vec3f(0,1,0), const float r = 1, const float R = 2);

    /// f:Unit Sphere -> R
    static ExplicitSurfaceFunction sphericalHarmonic(const std::function<float(Vec3f)>& f, const Vec3f& offset = Vec3f(0,0,0), float scale = 1.0f);
};

}
