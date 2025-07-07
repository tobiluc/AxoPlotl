#pragma once

#include "../utils/Utils.h"
#include "AxoPlotl/commons/Mesh.h"

namespace AxoPlotl
{

struct ExplicitSurfaceFunction
{
    std::function<Vec3f(float,float)> f;
    float uMin, uMax, vMin, vMax;

    inline Vec3f operator()(float u, float v) const {return f(u,v);}
};

typedef struct
{
    std::function<Vec3f(float)> f;
    float uMin, uMax;

} ExplicitCurveFunction;

struct ImplicitSurfaceFunction
{
    std::function<float(Vec3f)> f;
    float xMin, xMax, yMin, yMax, zMin, zMax;

    inline float operator()(const Vec3f& p) const {return f(p);}

    inline float operator()(float x, float y, float z) const {return f(Vec3f(x, y, z));}
};

ImplicitSurfaceFunction createUnion(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g);

ImplicitSurfaceFunction createIntersection(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g);

void createTriangles(const ExplicitSurfaceFunction& esf, TriangleMesh& mesh, const uint resolution = 32);

void createLines(const ExplicitCurveFunction& ecf, LineMesh& mesh, const uint resolution = 32);

void createTriangles(const ImplicitSurfaceFunction& isf, TriangleMesh& mesh, const uint resolution = 32);

class ExplicitSurfaceFunctionBuilder
{
public:
    static ExplicitSurfaceFunction sphere(const Vec3f& center = Vec3f(0,0,0), float radius = 1.0f);

    static ExplicitSurfaceFunction torus(const Vec3f& center = Vec3f(0,0,0), Vec3f axis = Vec3f(0,1,0), const float r = 1, const float R = 2);

    static ExplicitSurfaceFunction moebiusStrip(float R = 1);

    /// f:Unit Sphere -> R
    static ExplicitSurfaceFunction sphericalHarmonic(const std::function<float(Vec3f)>& f, const Vec3f& offset = Vec3f(0,0,0), float scale = 1.0f);
};

class ImplicitSurfaceFunctionBuilder
{
public:
    static ImplicitSurfaceFunction sphere(const Vec3f& center = Vec3f(0,0,0), float radius = 1.0f);

    static ImplicitSurfaceFunction torus(const float r = 1, const float R = 2);

    static ImplicitSurfaceFunction gyroid();

    static ImplicitSurfaceFunction heart();

    static ImplicitSurfaceFunction test();
};

}
