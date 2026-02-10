#pragma once

#include "AxoPlotl/commons/Mesh.h"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "AxoPlotl/typedefs/typedefs_glm.hpp"
#include "AxoPlotl/typedefs/typedefs_ToLoG.hpp"

namespace AxoPlotl
{

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

struct ImplicitSurfaceFunction
{
    std::function<float(Vec3f)> f;
    float xMin, xMax, yMin, yMax, zMin, zMax;
    std::string str;

    inline float operator()(const Vec3f& p) const {return f(p);}

    inline float operator()(float x, float y, float z) const {return f(Vec3f(x, y, z));}
};

struct SphericalHarmonicFunction
{
    std::function<float(Vec3f)> f;
    std::string str;

    inline float operator()(const Vec3f& p) const {return f(p);}

    inline float operator()(float x, float y, float z) const {return f(Vec3f(x, y, z));}
};

ImplicitSurfaceFunction createUnion(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g);

ImplicitSurfaceFunction createIntersection(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g);

void createMesh(const ExplicitSurfaceFunction& esf, VolumeMesh& mesh, const uint resolution = 32);

void createQuads(const ExplicitSurfaceFunction& esf, SurfaceMesh& mesh, const uint resolution = 32);

void createQuads(const SphericalHarmonicFunction& sh, SurfaceMesh& mesh, const uint resolution = 32);

void samplePoints(const ExplicitCurveFunction& ecf,
                  std::vector<std::pair<float,Vec3f>>& pts, const uint resolution = 32);

/// Marching Cubes
void createTrianglesMC(const ImplicitSurfaceFunction& isf,
    SurfaceMesh& mesh, const uint resolution = 32);

/// Adaptive MC (using octree)
void createTrianglesAMC(const ImplicitSurfaceFunction& isf, SurfaceMesh& mesh,
            const uint resolution = 8, const uint maxDepth = 4);

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

class SphericalHarmonicFunctionBuilder
{
public:
    static SphericalHarmonicFunction identityFrame();
};

class ImplicitSurfaceFunctionBuilder
{
public:
    static ImplicitSurfaceFunction dummy();

    static ImplicitSurfaceFunction sphere(const Vec3f& center = Vec3f(0,0,0), float radius = 1.0f);

    static ImplicitSurfaceFunction torus(const float r = 1, const float R = 2);

    static ImplicitSurfaceFunction gyroid();

    static ImplicitSurfaceFunction heart();

    static ImplicitSurfaceFunction cube();

    static ImplicitSurfaceFunction wineglass();

    static ImplicitSurfaceFunction test();
};

}
