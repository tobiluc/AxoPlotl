#pragma once

#include "AxoPlotl/commons/Mesh.h"
#include "AxoPlotl/geometry/ovm.h"

namespace AxoPlotl::IO
{

bool loadMesh(const std::string& filename, PolyhedralMesh& mesh);

}
