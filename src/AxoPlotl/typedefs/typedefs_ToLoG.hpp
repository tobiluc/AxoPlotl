#pragma once
#include <ToLoG/Traits_fwd.hpp>
#include "AxoPlotl/typedefs/typedefs_glm.hpp"
#include "ToLoG/mesh/polygon_mesh.hpp"

namespace ToLoG
{

template<typename FT, int DIM>
struct Traits<glm::vec<DIM,FT>>
{
    using value_type = FT;
    using vector_type = glm::vec<DIM,FT>;
    constexpr static int dim = DIM;
};
}

namespace AxoPlotl
{

using SurfaceMesh = ToLoG::PolygonMesh<Vec3f>;

}
