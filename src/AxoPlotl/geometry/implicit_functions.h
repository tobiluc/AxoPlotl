#pragma once

#include <functional>
#include <glm/fwd.hpp>
#include <glm/common.hpp>
#include <string>
#include <AxoPlotl/geometry/AABB.h>

namespace AxoPlotl::Geometry
{

struct ImplicitSurfaceFunction
{
    std::function<float(glm::vec3)> f;
    AABB bounds;
    std::string str;

    inline float operator()(const glm::vec3& p) const {
        return f(p);
    }

    inline float operator()(float x, float y, float z) const {
        return f(glm::vec3(x, y, z));
    }
};

class ImplicitSurfaceFunctionBuilder
{
public:
    static ImplicitSurfaceFunction dummy();

    static ImplicitSurfaceFunction sphere(const glm::vec3& center = glm::vec3(0,0,0), float radius = 1.0f);

    static ImplicitSurfaceFunction torus(const float r = 1, const float R = 2);

    static ImplicitSurfaceFunction gyroid();

    static ImplicitSurfaceFunction heart();

    static ImplicitSurfaceFunction cube();

    static ImplicitSurfaceFunction wineglass();

    static ImplicitSurfaceFunction test();
};

struct SphericalHarmonicFunction
{
    std::function<float(glm::vec3)> f;
    std::string str;

    inline float operator()(const glm::vec3& p) const {return f(p);}

    inline float operator()(float x, float y, float z) const {return f(glm::vec3(x, y, z));}
};

class SphericalHarmonicFunctionBuilder
{
public:
    static SphericalHarmonicFunction identityFrame();
};

}
