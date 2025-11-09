#pragma once

#include "AxoPlotl/geometry/ovm.h"

namespace AxoPlotl::IO
{

bool loadMesh(const std::filesystem::path& path, PolyhedralMesh& mesh);

}
