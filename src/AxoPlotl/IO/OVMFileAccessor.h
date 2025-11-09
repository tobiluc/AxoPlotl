#pragma once

#include "FileUtils.h"

#include "AxoPlotl/geometry/ovm.h"
#include "OpenVolumeMesh/IO/PropertyCodecs.hh"
#include "OpenVolumeMesh/IO/ovmb_read.hh"

namespace OpenVolumeMesh {

//template <> OVM_EXPORT const std::string typeName<IGRec::Eig::Mat3x3d>();

}

namespace AxoPlotl::IO
{

// bool loadMeshOVM(const std::string& filename, PolyhedralMesh& mesh);

// bool loadMeshOVMB(const std::string& filename, PolyhedralMesh& mesh);

bool loadMeshOVM(const std::filesystem::path& path, PolyhedralMesh& mesh);

//bool readTetMesh(const std::string& filename, TetrahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

//bool readHexMesh(const std::string& filename, HexahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

void writeMeshOVM(const std::filesystem::path& path, const PolyhedralMesh& mesh);

}
