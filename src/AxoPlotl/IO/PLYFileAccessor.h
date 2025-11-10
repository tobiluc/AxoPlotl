#pragma once

#include <AxoPlotl/commons/ovm.h>

namespace AxoPlotl::IO
{

bool loadMeshPLY(const std::string& filename, PolyhedralMesh& mesh);

}
