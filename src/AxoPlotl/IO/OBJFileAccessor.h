#pragma once

#include "AxoPlotl/geometry/ovm.h"

namespace AxoPlotl::IO
{

bool loadMeshOBJ(const std::filesystem::path&, PolyhedralMesh& mesh);

}
