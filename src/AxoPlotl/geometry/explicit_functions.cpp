#include "explicit_functions.h"
#include "AxoPlotl/algorithms/marching_cubes.h"
#include "AxoPlotl/IO/FileUtils.h"

namespace AxoPlotl::Geometry
{

void createMesh(const ExplicitSurfaceFunction& esf,
                std::vector<Vec3f>& points,
                std::vector<std::array<uint32_t,3>>& triangles,
                const uint32_t resolution)
{
    points.reserve((resolution+1)*(resolution+1));
    triangles.reserve(2*(resolution+1)*(resolution+1));

    // Generate Vertex Positions
    int vo = points.size(); // vertex offset
    float s, t;
    for (int i = 0; i <= resolution; ++i) {
        s = esf.uMin + i * (esf.uMax - esf.uMin) / resolution;
        for (int j = 0; j <= resolution; ++j) {
            t = esf.vMin + j * (esf.vMax - esf.vMin) / resolution;
            points.push_back(esf(s, t));
        }
    }

    // Generate Triangles
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            uint32_t row1 = i * (resolution + 1);
            uint32_t row2 = (i + 1) * (resolution + 1);

            triangles.push_back({
               (vo + row1 + j + 0),
               (vo + row2 + j + 0),
               (vo + row2 + j + 1)
            });

            triangles.push_back({
                (vo + row2 + j + 1),
                (vo + row1 + j + 1),
                (vo + row1 + j + 0)
            });
        }
    }
}

void samplePoints(const ExplicitCurveFunction& ecf, std::vector<std::pair<float,Vec3f>>& pts, const uint32_t resolution)
{
    pts.reserve(resolution+1);
    for (uint32_t i = 0; i <= resolution; ++i) {
        float t = ecf.tMin + i * (ecf.tMax - ecf.tMin) / resolution;
        Vec3f f = ecf.f(t);
        pts.emplace_back(t, f);
    }
}



ExplicitCurveFunction ExplicitCurveFunctionBuilder::dummy()
{
    return ExplicitCurveFunction{
        .f = [](float t)
        {
            return Vec3f(0,0,0);
        }, .tMin=0,.tMax=1,
            .str_x = "0",
            .str_y = "0",
            .str_z = "0"
    };
}

ExplicitCurveFunction ExplicitCurveFunctionBuilder::butterfly()
{
    return ExplicitCurveFunction{
        .f = [](float t)
        {
            float a = exp(cos(t)) - 2*cos(4*t) - pow(sin(t/12), 5);
            return Vec3f(
                sin(t)*a,
                cos(t)*a,
                0
            );
        }, .tMin=0,.tMax=12*M_PI,
            .str_x = "sin(t)*(exp(cos(t)) - 2*cos(4*t) - sin(t/12)^5)",
            .str_y = "cos(t)*(exp(cos(t)) - 2*cos(4*t) - sin(t/12)^5)",
            .str_z = "0"
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::dummy()
{
    return ExplicitSurfaceFunction{
        .f = [](float u, float v)
        {
            return Vec3f(0,0,0);
        }, .uMin=0,.uMax=1,.vMin=0,.vMax=1,
        .str_x = "0",
        .str_y = "0",
        .str_z = "0"
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::sphere(float radius)
{
    return ExplicitSurfaceFunction{
        .f = [radius](float phi, float theta)
        {
            Vec3f res = Vec3f(
                radius * sin(phi) * cos(theta),
                radius * cos(phi),
                radius * sin(phi) * sin(theta)
            );
            return res;
        }, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI,
        .str_x = IO::string_format("%f * sin(u)*cos(v)", radius),
        .str_y = IO::string_format("%f * cos(u)", radius),
        .str_z = IO::string_format("%f * sin(u)*sin(v)", radius)
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::torus(const float r, const float R)
{
    return ExplicitSurfaceFunction{
        .f = [R,r](float u, float v)
        {
            return Vec3f(
                (R + r*sin(u))*cos(v),
                (R + r*sin(u))*sin(v),
                r*cos(u)
            );
        }, .uMin=0,.uMax=2.0*M_PI,.vMin=0,.vMax=2.0*M_PI,
        .str_x = IO::string_format("(%f + %f*sin(u))*cos(v)", R, r),
        .str_y = IO::string_format("(%f + %f*sin(u))*sin(v)", R, r),
        .str_z = IO::string_format("%f*cos(u)", r)
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::moebiusStrip(float R)
{
    return ExplicitSurfaceFunction{
        .f = [R](float u, float v)
        {
            float u2 = u * 0.5;
            float v2 = v * 0.5;

            float t = R + v2*cos(u2);
            return Vec3f(
                t*cos(u),
                t*sin(u),
                v2*sin(u2)
            );
        }, .uMin=0,.uMax=2.0*M_PI,.vMin=-1.0f,.vMax=1.0f,
        .str_x = IO::string_format("(%f + 0.5*v*cos(0.5*u)) * cos(u)", R),
        .str_y = IO::string_format("(%f + 0.5*v*cos(0.5*u)) * sin(u)", R),
        .str_z = IO::string_format("0.5*v * sin(0.5*u)"),
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::sphericalHarmonic(const SphericalHarmonicFunction &sh)
{
    std::function<Vec3f(float,float)> func = [sh](float phi, float theta)
    {
        Vec3f p(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
        float r = sh(p);
        return r * p;
    };
    return ExplicitSurfaceFunction{
        .f = func, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI,
        .str_x = "",
        .str_y = "",
        .str_z = ""
    };
}





}
