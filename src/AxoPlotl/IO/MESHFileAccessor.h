#pragma once

#include <AxoPlotl/commons/ovm.h>
#include <filesystem>

namespace AxoPlotl::IO
{

bool loadMeshMESH(const std::filesystem::path& filepath, PolyhedralMesh& mesh);

}
