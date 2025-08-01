#pragma once

#include <AxoPlotl/geometry/ovm.h>
#include <filesystem>

namespace AxoPlotl::IO
{

bool loadMeshMESH(const std::filesystem::path& filepath, PolyhedralMesh& mesh);

}
