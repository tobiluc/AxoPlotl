#include "OVMFileAccessor.h"
#include <iomanip>

namespace AxoPlotl::IO
{

// bool readTetMesh(const std::string& filename, TetrahedralMesh& mesh, FileFormat ext)
// {
//     if (ext == FileFormat::INVALID) ext = getFileFormatFromName(filename);
//     if (ext == FileFormat::OVMA)
//     {
//         OVM::IO::FileManager fm;
//         return fm.readFile(filename, mesh);
//     }
//     else if (ext == FileFormat::OVMB)
//     {
//         auto codecs = OVM::IO::g_default_property_codecs;
//         auto res = OVM::IO::ovmb_read(filename.c_str(), mesh, OVM::IO::ReadOptions(), codecs);
//         if (res != OVM::IO::ReadResult::Ok) {std::cerr << OVM::IO::to_string(res) << std::endl; return false;}

//         return true;
//     }
//     else
//     {
//         std::cerr << "Unsupported File Format. Cannot read Input from: " << filename << std::endl;
//         return false;
//     }
// }

// bool readHexMesh(const std::string& filename, HexahedralMesh& mesh, FileFormat ext)
// {
//     if (ext == FileFormat::INVALID) ext = getFileFormatFromName(filename);
//     if (ext == FileFormat::OVMA)
//     {
//         OVM::IO::FileManager fm;
//         return fm.readFile(filename, mesh);
//     }
//     else if (ext == FileFormat::OVMB)
//     {
//         auto codecs = OVM::IO::g_default_property_codecs;
//         auto res = OVM::IO::ovmb_read(filename.c_str(), mesh, OVM::IO::ReadOptions(), codecs);
//         if (res != OVM::IO::ReadResult::Ok) {std::cerr << OVM::IO::to_string(res) << std::endl; return false;}

//         return true;
//     }
//     else
//     {
//         std::cerr << "Unsupported File Format. Cannot read Input from: " << filename << std::endl;
//         return false;
//     }
// }

void writeTetMesh(const std::string& filename, const TetrahedralMesh& mesh, FileFormat ext)
{
    if (ext == FileFormat::INVALID) ext = getFileFormatFromName(filename);

    if (ext == FileFormat::OVM)
    {
        OVM::IO::FileManager fm;
        std::ofstream off(filename.c_str(), std::ios::out);
        off << std::setprecision(100);
        if(!off.good()) std::cerr << "Could not open file " << filename << " for writing!" <<std::endl;
        else fm.writeStream(off, mesh);
    }
    else if (ext == FileFormat::OVMB)
    {
        // Save Binary
        auto codecs = OVM::IO::g_default_property_codecs;
        auto res = OVM::IO::ovmb_write(filename.c_str(), mesh, OVM::IO::WriteOptions(), codecs);
        if (res != OVM::IO::WriteResult::Ok) std::cerr << OVM::IO::to_string(res) << std::endl;
    }
    else
    {
        std::cerr << "Unsupported File Format. Cannot write Input Tet Mesh to: " << filename << std::endl;
    }
}
}
