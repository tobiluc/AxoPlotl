#pragma once

#include "AxoPlotl/commons/Mesh.h"

namespace AxoPlotl::IO
{

bool loadMeshPLY(const std::string& filename, Mesh& mesh);

}
