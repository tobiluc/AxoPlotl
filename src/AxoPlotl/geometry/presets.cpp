#include "presets.h"
#include "AxoPlotl/algorithms/marching_cubes.h"

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

void createTriangles(const ExplicitSurfaceFunction& esf, TriangleMesh &mesh, const uint resolution)
{
    // Generate Vertex Positions
    int vo = mesh.vertices.size(); // vertex offset
    float s, t;
    for (int i = 0; i <= resolution; ++i) {
        s = esf.uMin + i * (esf.uMax - esf.uMin) / resolution;
        for (int j = 0; j <= resolution; ++j) {
            t = esf.vMin + j * (esf.vMax - esf.vMin) / resolution;
            mesh.vertices.push_back(esf(s, t));
        }
    }

    // Generate Triangles
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int row1 = i * (resolution + 1);
            int row2 = (i + 1) * (resolution + 1);

            mesh.triangles.emplace_back(
                vo + row1 + j + 0,
                vo + row2 + j + 0,
                vo + row2 + j + 1
            );

            mesh.triangles.emplace_back(
                vo + row1 + j + 0,
                vo + row2 + j + 1,
                vo + row1 + j + 1
            );
        }
    }
}

void createLines(const ExplicitCurveFunction& ecf, LineMesh& mesh, const uint resolution)
{
    // Add Vertices
    int vo = mesh.vertices.size();
    for (uint i = 0; i < resolution; ++i) {
        float t = ecf.uMin + (i+1) * (ecf.uMax - ecf.uMin) / resolution;
        Vec3f f = ecf.f(t);

        mesh.vertices.push_back(f);
    }

    // Add Lines
    for (uint i = 0; i < resolution-1; ++i) {
        mesh.lines.emplace_back(vo + i + 0, vo + i + 1);
    }
}

void createTriangles(const ImplicitSurfaceFunction& isf, TriangleMesh &mesh, const uint resolution)
{
    Algo::MarchingCubes mc;
    mc.setBounds(isf.xMin, isf.xMax, isf.yMin, isf.yMax, isf.zMin, isf.zMax);
    mc.setResolution(resolution, resolution, resolution);
    mc.generate(isf.f, mesh);
}

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::sphere(const Vec3f& center, float radius)
{
    return ExplicitSurfaceFunction{
        .f = [center,radius](float phi, float theta)
        {
            Vec3f res = center + Vec3f(
                radius * sin(phi) * cos(theta),
                radius * cos(phi),
                radius * sin(phi) * sin(theta)
            );
            return res;
        }, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::torus(const Vec3f& center, Vec3f axis, const float r, const float R)
{
    axis = glm::normalize(axis);
    const Vec3f e1 = glm::normalize(cross(abs(axis[2]) < 0.99f ? Vec3f(0,0,1) : Vec3f(1,0,0), axis));
    const Vec3f e2 = cross(axis, e1);

    return ExplicitSurfaceFunction{
        .f = [center,R,r,e1,e2,axis](float u, float v)
        {
            return center
                   + (R + r * cos(v)) * (cos(u) * e1 + sin(u) * e2)
                   + r * sin(v) * axis;
        }, .uMin=0,.uMax=2.0*M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}

ExplicitSurfaceFunction ExplictSurfaceFunctionBuilder::sphericalHarmonic(const std::function<float(Vec3f)>& f, const Vec3f& offset, float scale)
{
    std::function<Vec3f(float,float)> func = [offset,scale,f](float phi, float theta)
    {
        Vec3f p(sin(phi) * cos(theta), cos(phi), sin(phi) * sin(theta));
        float r = f(p);
        return offset + scale * r * p;
    };
    return ExplicitSurfaceFunction{
        .f = func, .uMin=0,.uMax=M_PI,.vMin=0,.vMax=2.0*M_PI
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::heart()
{
    return ImplicitSurfaceFunction{
        .f = [](Vec3f p) {
            // x += 7; y += 7; z += 7;
            // return (x-2)*(x-2)*(x+2)*(x+2)
            //      + (y-2)*(y-2)*(y+2)*(y+2)
            //      + (z-2)*(z-2)*(z+2)*(z+2)
            //      + 3*(x*x*y*y + x*x*z*z + y*y*z*z)
            //        + 6*x*y*z - 10*(x*x+y*y+z*z) + 22;
            float x = p.x/5;
            float y = p.y/5;
            float z = p.z/5;
            float t = (2*x*x + 2*y*y + z*z - 1);
            return t*t*t - 0.1f*x*x*z*z*z - y*y*z*z*z;
        },
        .xMin = -10.f, .xMax = 10.f,
        .yMin = -10.f, .yMax = 10.f,
        .zMin = -10.f, .zMax = 10.f
    };
}

}
