#include "FileAccessor.h"
#include "AxoPlotl/IO/FileUtils.h"
#include "AxoPlotl/IO/OBJFileAccessor.h"
#include "AxoPlotl/IO/PLYFileAccessor.h"

bool AxoPlotl::IO::loadMesh(const std::string& filename, Mesh& mesh)
{
    FileFormat f = getFileFormatFromName(filename);
    switch(f) {
    case OBJ: return loadMeshOBJ(filename, mesh);
    case PLY: return loadMeshPLY(filename, mesh);
    case OVM:
    case OVMB:
    default: return false;
    }
}
