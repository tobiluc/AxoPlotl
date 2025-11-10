#include "ovm.h"

namespace AxoPlotl
{

bool is_tet_mesh(const PolyhedralMesh& _mesh)
{
    for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {
        const auto& hfhs = _mesh.cell(*c_it).halffaces();
        if (hfhs.size() != 4) {return false;}
        for (OVM::HFH hfh : hfhs) {
            if (_mesh.valence(hfh.face_handle()) != 3) {
                return false;
            }
        }
    }
    return true;
}

bool is_hex_mesh(const PolyhedralMesh& _mesh)
{
    for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {
        const auto& hfhs = _mesh.cell(*c_it).halffaces();
        if (hfhs.size() != 6) {return false;}
        for (OVM::HFH hfh : hfhs) {
            if (_mesh.valence(hfh.face_handle()) != 4) {
                return false;
            }
        }
    }
    return true;
}

}
