#pragma once

#include <filesystem>
#include <string>
#include <unistd.h>

namespace AxoPlotl::IO
{

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

enum FileFormat : char16_t {
    INVALID, OBJ, PLY, MESH, OVM, OVMB
};

inline FileFormat getFileFormatFromName(const std::filesystem::path& _path)
{
    std::string ext = _path.extension();
    if (ext == ".ovm") return FileFormat::OVM;
    if (ext == ".ovmb") return FileFormat::OVMB;
    if (ext == ".mesh") return FileFormat::MESH;
    if (ext == ".obj") return FileFormat::OBJ;
    if (ext == ".ply") return FileFormat::PLY;
    return FileFormat::INVALID;
}

inline std::string getWorkingDirectory()
{
    const size_t size = 1024;
    char buffer[size];
    if (getcwd(buffer, size) != NULL) return buffer;
    return "ERROR";
}

}
