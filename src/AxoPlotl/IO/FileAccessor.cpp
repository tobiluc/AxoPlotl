#include "FileAccessor.h"
#include "AxoPlotl/IO/MESHFileAccessor.h"
#include "AxoPlotl/IO/OBJFileAccessor.h"
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "AxoPlotl/IO/PLYFileAccessor.h"

bool AxoPlotl::IO::loadMesh(const std::filesystem::path& path, PolyhedralMesh &mesh)
{
    std::string ext = path.extension();
    if (ext == ".obj") return loadMeshOBJ(path, mesh);
    if (ext == ".ply") return loadMeshPLY(path, mesh);
    if (ext == ".ovm" || ext == ".ovmb") return loadMeshOVM(path, mesh);
    if (ext == ".mesh") return loadMeshMESH(path, mesh);
    std::cerr << "Unknown file format: " << ext << std::endl;
    return false;
}
