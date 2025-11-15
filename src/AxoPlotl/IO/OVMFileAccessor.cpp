#include "OVMFileAccessor.h"
#include <iomanip>
#include <Eigen/Core>
#include <Eigen/LU>
#include "OpenVolumeMesh/IO/ovmb_write.hh"
#include "OpenVolumeMesh/IO/ovmb_read.hh"
#include "OpenVolumeMesh/IO/PropertyCodecsEigen.hh"
#include "OpenVolumeMesh/IO/PropertyCodecs.hh"

namespace AxoPlotl::IO
{

OpenVolumeMesh::IO::PropertyCodecs ovm_property_codecs()
{
    using namespace OpenVolumeMesh::IO;
    auto codecs = g_default_property_codecs;
    OpenVolumeMesh::IO::register_eigen_codecs(codecs);
    return codecs;
}

bool loadMeshOVM(const std::filesystem::path& path, PolyhedralMesh& mesh)
{
    const std::string ext = path.extension();
    if (ext == ".ovm") {
        OVM::IO::FileManager fm;
        return fm.readFile(path, mesh);
    } else if (ext == ".ovmb") {
        auto res = OVM::IO::ovmb_read(path, mesh, OVM::IO::ReadOptions(), ovm_property_codecs());
        if (res != OVM::IO::ReadResult::Ok) {
            std::cerr << OVM::IO::to_string(res) << std::endl;
            return false;
        }
        return true;
    } else {
        return false;
    }
}

void writeMeshOVM(const std::filesystem::path& path, const PolyhedralMesh& mesh)
{
    const std::string ext = path.extension();
    if (ext == ".ovm") {
        OVM::IO::FileManager fm;
        std::ofstream off(path, std::ios::out);
        off << std::setprecision(100);
        if(!off.good()) std::cerr << "Could not open file " << path << " for writing!" <<std::endl;
        else fm.writeStream(off, mesh);
    } else if (ext == ".ovmb")
    {
        // Save Binary
        auto res = OVM::IO::ovmb_write(path, mesh, OVM::IO::WriteOptions(), ovm_property_codecs());
        if (res != OVM::IO::WriteResult::Ok) std::cerr << OVM::IO::to_string(res) << std::endl;
    }
    std::cerr << "Unsupported File Format. Cannot write Input Tet Mesh to: " << path << std::endl;
}

}
