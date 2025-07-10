#pragma once

#include "AxoPlotl/geometry/ovm.h"

namespace AxoPlotl::IO
{

bool loadMeshPLY(const std::string& filename, PolyhedralMesh& mesh);

}
