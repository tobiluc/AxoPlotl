#pragma once

#include "AxoPlotl/commons/ovm.h"

namespace AxoPlotl::IO
{

bool loadMeshOBJ(const std::filesystem::path&, PolyhedralMesh& mesh);

}
