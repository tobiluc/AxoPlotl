#pragma once

#include "Typedefs.h"
#include "Utils.h"

#include "OpenVolumeMesh/IO/ovmb_write.hh"
#include "OpenVolumeMesh/IO/PropertyCodecs.hh"
#include "OpenVolumeMesh/IO/PropertyCodecsT_impl.hh"
#include "OpenVolumeMesh/IO/ovmb_read.hh"


namespace MV
{
    enum FileFormat : char16_t {INVALID, MESH, OVMA, OVMB};

    inline FileFormat getFileFormatFromName(const std::string& filename)
    {
        std::string ext = toLower(splitFilenameExtension(filename).second);
        if (ext == ".ovm") return FileFormat::OVMA;
        if (ext == ".ovmb") return FileFormat::OVMB;
        if (ext == ".mesh") return FileFormat::MESH;
        return FileFormat::INVALID;
    }

    bool readTetMesh(const std::string& filename, TetrahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

    bool readHexMesh(const std::string& filename, HexahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

    void writeTetMesh(const std::string& filename, const TetrahedralMesh& mesh, FileFormat ext = FileFormat::INVALID);

}
