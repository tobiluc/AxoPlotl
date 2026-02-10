#include "Surface.h"
#include "AxoPlotl/algorithms/marching_cubes.h"
#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "AxoPlotl/utils/string_format.hpp"

namespace AxoPlotl
{

ImplicitSurfaceFunction createUnion(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g)
{
    return ImplicitSurfaceFunction{
        .f = [&](Vec3f p) {return f(p)*g(p);},
        .xMin = std::min(f.xMin, g.xMin),
        .xMax = std::max(f.xMax, g.xMax),
        .yMin = std::min(f.yMin, g.yMin),
        .yMax = std::max(f.yMax, g.yMax),
        .zMin = std::min(f.zMin, g.zMin),
        .zMax = std::max(f.zMax, g.zMax)
    };
}

ImplicitSurfaceFunction createIntersection(const ImplicitSurfaceFunction& f, ImplicitSurfaceFunction& g)
{
    return ImplicitSurfaceFunction{
        .f = [&](Vec3f p) {
            float fp = f(p);
            float gp = g(p);
            float a = std::max(std::abs(fp), std::abs(gp));
            return (fp*gp>0)? +a : (fp*gp<0)? -a : 0;
        },
        .xMin = std::max(f.xMin, g.xMin),
        .xMax = std::min(f.xMax, g.xMax),
        .yMin = std::max(f.yMin, g.yMin),
        .yMax = std::min(f.yMax, g.yMax),
        .zMin = std::max(f.zMin, g.zMin),
        .zMax = std::min(f.zMax, g.zMax)
    };
}

void createMesh(const ExplicitSurfaceFunction& esf, VolumeMesh &mesh, const uint resolution)
{
    // Generate Vertex Positions
    int vo = mesh.n_vertices(); // vertex offset
    float s, t;
    for (int i = 0; i <= resolution; ++i) {
        s = esf.uMin + i * (esf.uMax - esf.uMin) / resolution;
        for (int j = 0; j <= resolution; ++j) {
            t = esf.vMin + j * (esf.vMax - esf.vMin) / resolution;
            mesh.add_vertex(toVec3<OpenVolumeMesh::Vec3f>(esf(s, t)));
        }
    }

    // Generate Triangles
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int row1 = i * (resolution + 1);
            int row2 = (i + 1) * (resolution + 1);

            mesh.add_face({
               OpenVolumeMesh::VH(vo + row1 + j + 0),
               OpenVolumeMesh::VH(vo + row2 + j + 0),
               OpenVolumeMesh::VH(vo + row2 + j + 1),
                OpenVolumeMesh::VH(vo + row1 + j + 1)
            });
        }
    }
}

void createQuads(const ExplicitSurfaceFunction& esf, SurfaceMesh& mesh, const uint resolution)
{
    mesh.clear();

    // Generate Vertex Positions
    float s, t;
    for (int i = 0; i <= resolution; ++i) {
        s = esf.uMin + i * (esf.uMax - esf.uMin) / resolution;
        for (int j = 0; j <= resolution; ++j) {
            t = esf.vMin + j * (esf.vMax - esf.vMin) / resolution;
            mesh.add_vertex((esf(s, t)));
        }
    }

    // Generate Triangles
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int row1 = i * (resolution + 1);
            int row2 = (i + 1) * (resolution + 1);

            mesh.add_face(std::vector<int>{
                (row1 + j + 0),
                (row2 + j + 0),
                (row2 + j + 1),
                (row1 + j + 1)
            });
        }
    }
}

void createQuads(const SphericalHarmonicFunction& sh, SurfaceMesh& mesh, const uint resolution)
{
    createQuads(ExplicitSurfaceFunctionBuilder::sphericalHarmonic(sh), mesh, resolution);
}

void samplePoints(const ExplicitCurveFunction& ecf, std::vector<std::pair<float,Vec3f>>& pts, const uint resolution)
{
    pts.reserve(resolution+1);
    for (uint i = 0; i <= resolution; ++i) {
        float t = ecf.tMin + i * (ecf.tMax - ecf.tMin) / resolution;
        Vec3f f = ecf.f(t);
        pts.emplace_back(t, f);
    }
}

void createTrianglesMC(const ImplicitSurfaceFunction& isf, SurfaceMesh &mesh, const uint resolution)
{
    Algo::MarchingCubes mc;
    AABB b{isf.xMin, isf.xMax, isf.yMin, isf.yMax, isf.zMin, isf.zMax};
    mc.setBounds(b);
    mc.setResolution(resolution, resolution, resolution);
    mc.generate(isf.f, mesh);
}

void createTrianglesAMC(const ImplicitSurfaceFunction& isf, SurfaceMesh& mesh,
                        const uint resolution, const uint maxDepth)
{
    Algo::MarchingCubes mc;
    AABB b{isf.xMin, isf.xMax, isf.yMin, isf.yMax, isf.zMin, isf.zMax};
    mc.setBounds(b);
    mc.setResolution(resolution, resolution, resolution);
    mc.generateAdaptive(isf.f, mesh, maxDepth);
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
        .str_x = string_format("%f * sin(u)*cos(v)", radius),
        .str_y = string_format("%f * cos(u)", radius),
        .str_z = string_format("%f * sin(u)*sin(v)", radius)
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
        .str_x = string_format("(%f + %f*sin(u))*cos(v)", R, r),
        .str_y = string_format("(%f + %f*sin(u))*sin(v)", R, r),
        .str_z = string_format("%f*cos(u)", r)
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
        .str_x = string_format("(%f + 0.5*v*cos(0.5*u)) * cos(u)", R),
        .str_y = string_format("(%f + 0.5*v*cos(0.5*u)) * sin(u)", R),
        .str_z = string_format("0.5*v * sin(0.5*u)"),
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

SphericalHarmonicFunction SphericalHarmonicFunctionBuilder::identityFrame()
{
    return SphericalHarmonicFunction{
        .f = [](Vec3f p) {
            return pow(p.x,4)+pow(p.y,4)+pow(p.z,4);
        },
        .str = "x^4 + y^4 + z^4"
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::heart()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            float x = p.x;
            float y = p.y;
            float z = p.z;
            float t = x*x + (9.f/4.f)*z*z + y*y - 1;
            return t*t*t - x*x*y*y*y - (9.f/80.f)*y*y*y*z*z;
        },
        .xMin = -2.f, .xMax = 2.f,
        .yMin = -2.f, .yMax = 2.f,
        .zMin = -2.f, .zMax = 2.f,
        .str = "(x*x + (9.0/4.0)*z*z + y*y - 1)^3 - x*x*y*y*y - (9.0/80.0)*y*y*y*z*z"
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::dummy()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            return 0;
        },
        .xMin = 0, .xMax = 1,
        .yMin = 0, .yMax = 1,
        .zMin = 0, .zMax = 1,
        .str = "0"
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::cube()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            return std::max(std::abs(p.x),std::max(std::abs(p.y),std::abs(p.z))) - 1;
        },
        .xMin = -1.5f, .xMax = 1.5f,
        .yMin = -1.5f, .yMax = 1.5f,
        .zMin = -1.5f, .zMax = 1.5f,
        .str = string_format("max(abs(x),abs(y),abs(z)) - 1")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::sphere(const Vec3f& center, float radius)
{
    return ImplicitSurfaceFunction{
        .f = [center,radius](Vec3f p) {
            float x = p.x - center.x;
            float y = p.y - center.y;
            float z = p.z - center.z;
            return x*x + y*y + z*z - radius*radius;
        },
        .xMin = center.x - radius, .xMax = center.x + radius,
        .yMin = center.y - radius, .yMax = center.y + radius,
        .zMin = center.z - radius, .zMax = center.z + radius,
        .str = string_format("(x-%f)^2 + (y-%f)^2 + (z-%f)^2 - %f^2", center.x, center.y, center.z, radius)
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::torus(const float r, const float R)
{
    return ImplicitSurfaceFunction{
        .f = [r,R](Vec3f p) {
            float x = p.x;
            float y = p.y;
            float z = p.z;
            float t = (R - sqrt(x*x+z*z));
            return t*t + y*y - r*r;
        },
        .xMin = -r-R, .xMax = r+R,
        .yMin = -r-R, .yMax = r+R,
        .zMin = -r-R, .zMax = r+R,
        .str = string_format("(%f - (x*x+z*z)^0.5)^2 + y^2 - %f^2", R, r)
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::gyroid()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            float x = p.x, y = p.y, z = p.z;
            return sin(x)*cos(y)
                   + sin(y)*cos(z) + sin(z)*cos(x);
        },
        .xMin = -10.f, .xMax = 10.f,
        .yMin = -10.f, .yMax = 10.f,
        .zMin = -10.f, .zMax = 10.f,
        .str = string_format("sin(x)*cos(y) + sin(y)*cos(z) + sin(z)*cos(x)")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::wineglass()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            float t = (std::log(p.y + 3.2f));
            return p.x*p.x + p.z*p.z - t*t - 0.02f;
        },
        .xMin = -2, .xMax = 2,
        .yMin = -3.0f, .yMax = 3,
        .zMin = -2, .zMax = 2,
        .str = string_format("x^2 + z^2 - (ln(y + 3.2))^2 - 0.02")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::test()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            float x = p.x, y = p.y, z = p.z;
            return (x-2)*(x-2)*(x+2)*(x+2)
                 + (y-2)*(y-2)*(y+2)*(y+2)
                 + (z-2)*(z-2)*(z+2)*(z+2)
                 + 3*(x*x*y*y + x*x*z*z + y*y*z*z)
                   + 6*x*y*z - 10*(x*x+y*y+z*z) + 22;
        },
        .xMin = -10.f, .xMax = 10.f,
        .yMin = -10.f, .yMax = 10.f,
        .zMin = -10.f, .zMax = 10.f,
        .str = string_format("(x-2)*(x-2)*(x+2)*(x+2) + (y-2)*(y-2)*(y+2)*(y+2) + (z-2)*(z-2)*(z+2)*(z+2) + 3*(x*x*y*y + x*x*z*z + y*y*z*z) + 6*x*y*z - 10*(x*x+y*y+z*z) + 22")
    };
}

}
