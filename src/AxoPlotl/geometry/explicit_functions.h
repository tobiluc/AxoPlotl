#pragma once

#include "AxoPlotl/geometry/implicit_functions.h"

namespace AxoPlotl::Geometry
{

using Vec3f = glm::vec3;

struct ExplicitSurfaceFunction
{
    std::function<Vec3f(float,float)> f;
    float uMin=-1, uMax=1, vMin=-1, vMax=1;
    std::string str_x = "u", str_y = "u^2 + v^2", str_z = "v";

    inline Vec3f operator()(float u, float v) const {return f(u,v);}
};

struct ExplicitCurveFunction
{
    std::function<Vec3f(float)> f;
    float tMin, tMax;
    std::string str_x = "cos(t)", str_y = "t", str_z = "sin(t)";

    inline Vec3f operator()(float t) const {return f(t);}

};

void createMesh(const ExplicitSurfaceFunction& esf,
                std::vector<Vec3f>& points,
                std::vector<std::array<uint32_t,3>>& triangles,
                const uint32_t resolution = 32);

//void createQuads(const ExplicitSurfaceFunction& esf, PolygonMesh& mesh, const uint resolution = 32);

//void createQuads(const SphericalHarmonicFunction& sh, PolygonMesh& mesh, const uint resolution = 32);

//void createTriangles(const ExplicitSurfaceFunction& esf, TriangleMesh& mesh, const uint resolution = 32);

void samplePoints(const ExplicitCurveFunction& ecf, std::vector<std::pair<float,Vec3f>>& pts, const uint32_t resolution = 32);

class ExplicitCurveFunctionBuilder
{
public:
    static ExplicitCurveFunction dummy();

    static ExplicitCurveFunction butterfly();
};

class ExplicitSurfaceFunctionBuilder
{
public:
    static ExplicitSurfaceFunction dummy();

    static ExplicitSurfaceFunction sphere(float radius = 1.0f);

    static ExplicitSurfaceFunction torus(const float r = 1, const float R = 2);

    static ExplicitSurfaceFunction moebiusStrip(float R = 1);

    /// f:Unit Sphere -> R
    static ExplicitSurfaceFunction sphericalHarmonic(const SphericalHarmonicFunction& sh);
};





}
