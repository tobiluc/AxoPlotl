#pragma once

#include "ToLoG/mesh/polygon_mesh_concepts.hpp"
#include <ToLoG/utils/OVM_Traits.hpp>
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl
{

namespace OVM = OpenVolumeMesh;

using VolumeMesh = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f, OpenVolumeMesh::TopologyKernel>;;

template<ToLoG::polygon_mesh_3 Mesh>
VolumeMesh volume_mesh(const Mesh& _mesh)
{
    using VI = typename ToLoG::Traits<Mesh>::vertex_index;
    VolumeMesh res;

    res.reserve_vertices(_mesh.n_vertices());
    for (int i = 0; i < _mesh.n_vertices(); ++i) {
        res.add_vertex(toVec3<OVM::Vec3f>(_mesh.point(VI(i))));
    }
    res.reserve_edges(_mesh.n_edges());
    for (const auto& e : _mesh.edges()) {
        res.add_edge(OVM::VH(ToLoG::index(e.vertex(0))),
                     OVM::VH(ToLoG::index(e.vertex(1))));
    }
    res.reserve_faces(_mesh.n_faces());
    for (const auto& f : _mesh.faces()) {
        std::vector<OVM::VH> vhs;
        for (const auto& v : f.vertices()) {
            vhs.push_back(OVM::VH(ToLoG::index(v)));
        }
        res.add_face(vhs);
    }

    return res;
}

}
