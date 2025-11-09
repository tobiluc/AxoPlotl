#pragma once

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

// inline std::string toLower(const std::string& s)
// {
//     std::string r = s;
//     std::transform(r.begin(), r.end(), r.begin(),[](unsigned char c){return std::tolower(c);});
//     return r;
// }

// inline std::pair<std::string,std::string> splitFilenameExtension(const std::string& filename)
// {
//     auto i = filename.find_last_of(".");
//     return std::make_pair(filename.substr(0,i), filename.substr(i));
// }

// enum FileFormat : char16_t {
//     INVALID, OBJ, PLY, MESH, OVM, OVMB
// };

// inline FileFormat getFileFormatFromName(const std::string& filename)
// {
//     std::string ext = toLower(splitFilenameExtension(filename).second);
//     if (ext == ".ovm") return FileFormat::OVM;
//     if (ext == ".ovmb") return FileFormat::OVMB;
//     if (ext == ".mesh") return FileFormat::MESH;
//     if (ext == ".obj") return FileFormat::OBJ;
//     if (ext == ".ply") return FileFormat::PLY;
//     return FileFormat::INVALID;
// }

inline std::string getWorkingDirectory()
{
    const size_t size = 1024;
    char buffer[size];
    if (getcwd(buffer, size) != NULL) return buffer;
    return "ERROR";
}

}
