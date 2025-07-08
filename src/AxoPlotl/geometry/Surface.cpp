#include "Surface.h"
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
            mesh.addVertex(esf(s, t));
        }
    }

    // Generate Triangles
    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int row1 = i * (resolution + 1);
            int row2 = (i + 1) * (resolution + 1);

            mesh.addTriangle(
                vo + row1 + j + 0,
                vo + row2 + j + 0,
                vo + row2 + j + 1
            );

            mesh.addTriangle(
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
        mesh.addLine(vo + i + 0, vo + i + 1);
    }
}

void createTriangles(const ImplicitSurfaceFunction& isf, TriangleMesh &mesh, const uint resolution)
{
    Algo::MarchingCubes mc;
    mc.setBounds(isf.xMin, isf.xMax, isf.yMin, isf.yMax, isf.zMin, isf.zMax);
    mc.setResolution(resolution, resolution, resolution);
    mc.generate(isf.f, mesh);
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::sphere(const Vec3f& center, float radius)
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

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::torus(const Vec3f& center, Vec3f axis, const float r, const float R)
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
        }, .uMin=0,.uMax=2.0*M_PI,.vMin=-1.0f,.vMax=1.0f
    };
}

ExplicitSurfaceFunction ExplicitSurfaceFunctionBuilder::sphericalHarmonic(const std::function<float(Vec3f)>& f, const Vec3f& offset, float scale)
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
            float x = p.x;
            float y = p.y;
            float z = p.z;
            float t = x*x + (9.f/4.f)*z*z + y*y - 1;
            return t*t*t - x*x*y*y*y - (9.f/80.f)*y*y*y*z*z;
        },
        .xMin = -2.f, .xMax = 2.f,
        .yMin = -2.f, .yMax = 2.f,
        .zMin = -2.f, .zMax = 2.f
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
        .zMin = center.z - radius, .zMax = center.z + radius
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
        .zMin = -r-R, .zMax = r+R
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
        .zMin = -10.f, .zMax = 10.f
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
        .zMin = -10.f, .zMax = 10.f
    };
}

}
