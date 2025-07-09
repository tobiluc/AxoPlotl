#pragma once

#include "FileUtils.h"

#include "AxoPlotl/geometry/ovm.h"
#include "OpenVolumeMesh/IO/ovmb_write.hh"
#include "OpenVolumeMesh/IO/PropertyCodecs.hh"
#include "OpenVolumeMesh/IO/PropertyCodecsT_impl.hh"
#include "OpenVolumeMesh/IO/ovmb_read.hh"


namespace AxoPlotl::IO
{

template<typename MeshT>
bool readMesh(const std::string& filename, MeshT& mesh, FileFormat ext = FileFormat::INVALID)
{
    if (ext == FileFormat::INVALID) ext = getFileFormatFromName(filename);
    if (ext == FileFormat::OVM)
    {
        OVM::IO::FileManager fm;
        return fm.readFile(filename, mesh);
    }
    else if (ext == FileFormat::OVMB)
    {
        auto codecs = OVM::IO::g_default_property_codecs;
        auto res = OVM::IO::ovmb_read(filename.c_str(), mesh, OVM::IO::ReadOptions(), codecs);
        if (res != OVM::IO::ReadResult::Ok) {std::cerr << OVM::IO::to_string(res) << std::endl; return false;}

        return true;
    }
    else
    {
        std::cerr << "Unsupported File Format. Cannot read Input from: " << filename << std::endl;
        return false;
    }
}

//bool readTetMesh(const std::string& filename, TetrahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

//bool readHexMesh(const std::string& filename, HexahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

void writeTetMesh(const std::string& filename, const TetrahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

}
