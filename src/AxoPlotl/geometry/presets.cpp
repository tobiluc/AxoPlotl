#include "presets.h"

namespace AxoPlotl
{

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::sphere(const Vec3f& center, float radius)
{
    return ExplicitSurfaceFunction{
        .f = [&](float phi, float theta)
        {
            return center + Vec3f(
                radius * sin(phi) * cos(theta),
                radius * cos(phi),
                radius * sin(phi) * sin(theta)
            );
        }, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::torus(const Vec3f& center, Vec3f axis, const float r, const float R)
{
    axis = glm::normalize(axis);
    const Vec3f e1 = glm::normalize(cross(abs(axis[2]) < 0.99f ? Vec3f(0,0,1) : Vec3f(1,0,0), axis));
    const Vec3f e2 = cross(axis, e1);

    return ExplicitSurfaceFunction{
        .f = [&](float u, float v)
        {
            return center
                   + (R + r * cos(v)) * (cos(u) * e1 + sin(u) * e2)
                   + r * sin(v) * axis;
        }, .uMin=0,.uMax=2.0*M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::sphericalHarmonic(const std::function<float(Vec3f)>& f, const Vec3f& offset, float scale)
{
    std::function<Vec3f(float,float)> func = [&](float phi, float theta)
    {
        Vec3f p(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
        float r = f(p);
        return offset + scale * r * p;
    };
    return ExplicitSurfaceFunction{
        .f = func, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}



}
