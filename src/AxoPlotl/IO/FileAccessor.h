#pragma once

#include "AxoPlotl/typedefs/typedefs_OpenVolumeMesh.hpp"
#include "AxoPlotl/typedefs/typedefs_ToLoG.hpp"
#include <filesystem>

namespace AxoPlotl::IO
{

std::optional<std::variant<SurfaceMesh,VolumeMesh>> read_mesh(
    const std::filesystem::path& _path);

}
