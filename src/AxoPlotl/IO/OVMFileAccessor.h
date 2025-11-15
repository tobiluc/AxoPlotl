#pragma once

#include "AxoPlotl/commons/ovm.h"
#include "OpenVolumeMesh/IO/PropertyCodecs.hh"

namespace AxoPlotl::IO
{

OpenVolumeMesh::IO::PropertyCodecs ovm_property_codecs();

bool loadMeshOVM(const std::filesystem::path& path, PolyhedralMesh& mesh);

void writeMeshOVM(const std::filesystem::path& path, const PolyhedralMesh& mesh);

}
