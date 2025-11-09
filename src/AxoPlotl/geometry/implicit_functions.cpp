#include "implicit_functions.h"
#include "AxoPlotl/IO/FileUtils.h"

namespace AxoPlotl::Geometry
{

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::heart()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            float x = p.x;
            float y = p.y;
            float z = p.z;
            float t = x*x + (9.f/4.f)*z*z + y*y - 1;
            return t*t*t - x*x*y*y*y - (9.f/80.f)*y*y*y*z*z;
        },
        .bounds = {-2,2,-2,2,-2,2},
        .str = "(x*x + (9.0/4.0)*z*z + y*y - 1)^3 - x*x*y*y*y - (9.0/80.0)*y*y*y*z*z"
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::dummy()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            return 0;
        },
        .bounds = {0,1,0,1,0,1},
        .str = "0"
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::cube()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            return std::max(std::abs(p.x),std::max(std::abs(p.y),std::abs(p.z))) - 1;
        },
        .bounds = {-1.5,1.5,-1.5,1.5,-1.5,1.5},
        .str = IO::string_format("max(abs(x),abs(y),abs(z)) - 1")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::sphere(const glm::vec3& center, float radius)
{
    return ImplicitSurfaceFunction{
        .f = [center,radius](glm::vec3 p) {
            float x = p.x - center.x;
            float y = p.y - center.y;
            float z = p.z - center.z;
            return x*x + y*y + z*z - radius*radius;
        },
        .bounds = {center.x - radius,center.x + radius,
            center.y - radius, center.y + radius,
                   center.z - radius, center.z + radius},
        .str = IO::string_format("(x-%f)^2 + (y-%f)^2 + (z-%f)^2 - %f^2", center.x, center.y, center.z, radius)
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::torus(const float r, const float R)
{
    return ImplicitSurfaceFunction{
        .f = [r,R](glm::vec3 p) {
            float x = p.x;
            float y = p.y;
            float z = p.z;
            float t = (R - sqrt(x*x+z*z));
            return t*t + y*y - r*r;
        },
        .bounds = {-r-R,r+R,-r-R,r+R,-r-R,r+R},
        .str = IO::string_format("(%f - (x*x+z*z)^0.5)^2 + y^2 - %f^2", R, r)
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::gyroid()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            float x = p.x, y = p.y, z = p.z;
            return sin(x)*cos(y)
                   + sin(y)*cos(z) + sin(z)*cos(x);
        },
        .bounds = {-10,10,-10,10,-10,10},
        .str = IO::string_format("sin(x)*cos(y) + sin(y)*cos(z) + sin(z)*cos(x)")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::wineglass()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            float t = (std::log(p.y + 3.2f));
            return p.x*p.x + p.z*p.z - t*t - 0.02f;
        },
        .bounds = {-2,2,-3,3,-2,2},
        .str = IO::string_format("x^2 + z^2 - (ln(y + 3.2))^2 - 0.02")
    };
}

ImplicitSurfaceFunction ImplicitSurfaceFunctionBuilder::test()
{
    return ImplicitSurfaceFunction{
        .f = [](glm::vec3 p) {
            float x = p.x, y = p.y, z = p.z;
            return (x-2)*(x-2)*(x+2)*(x+2)
                   + (y-2)*(y-2)*(y+2)*(y+2)
                   + (z-2)*(z-2)*(z+2)*(z+2)
                   + 3*(x*x*y*y + x*x*z*z + y*y*z*z)
                   + 6*x*y*z - 10*(x*x+y*y+z*z) + 22;
        },
        .bounds = {-10,10,-10,10,-10,10},
        .str = IO::string_format("(x-2)*(x-2)*(x+2)*(x+2) + (y-2)*(y-2)*(y+2)*(y+2) + (z-2)*(z-2)*(z+2)*(z+2) + 3*(x*x*y*y + x*x*z*z + y*y*z*z) + 6*x*y*z - 10*(x*x+y*y+z*z) + 22")
    };
}

SphericalHarmonicFunction SphericalHarmonicFunctionBuilder::identityFrame()
{
    return SphericalHarmonicFunction{
        .f = [](glm::vec3 p) {
            return pow(p.x,4)+pow(p.y,4)+pow(p.z,4);
        },
        .str = "x^4 + y^4 + z^4"
    };
}

}
