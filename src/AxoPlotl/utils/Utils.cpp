#include "Utils.h"

namespace AxPl
{
    int mesh_n_boundary_faces(TetrahedralMesh& mesh)
    {
        int n = 0;
        for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it)
            if (mesh.is_boundary(*f_it))
                n++;
        return n;
    }
}
