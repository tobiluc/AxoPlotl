#pragma once

#include "AxoPlotl/commons/ovm.h"

namespace AxoPlotl::IO
{

bool loadMesh(const std::filesystem::path& path, PolyhedralMesh& mesh);

}
