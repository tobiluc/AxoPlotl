#include "FileAccessor.h"
#include "AxoPlotl/IO/FileUtils.h"
#include "AxoPlotl/IO/MESHFileAccessor.h"
#include "AxoPlotl/IO/OBJFileAccessor.h"
#include "AxoPlotl/IO/OVMFileAccessor.h"
#include "AxoPlotl/IO/PLYFileAccessor.h"

bool AxoPlotl::IO::loadMesh(const std::string& filename, PolyhedralMesh &mesh)
{
    FileFormat f = getFileFormatFromName(filename);
    switch(f) {
    case OBJ: return loadMeshOBJ(filename, mesh);
    case PLY: return loadMeshPLY(filename, mesh);
    case OVM: return loadMeshOVM(filename, mesh);
    case OVMB: return loadMeshOVMB(filename, mesh);
    case MESH: return loadMeshMESH(filename, mesh);
    default: return false;
    }
}
