#include "polyscope_register_ovm_mesh.h"
#include "polyscope/curve_network.h"

namespace AxoPlotl
{

polyscope::PointCloud* Interface::register_point_cloud(const std::string& _name, const PolyhedralMesh& _mesh)
{
    if (_mesh.n_vertices()==0) {return nullptr;}
    polyscope::PointCloud* pc = polyscope::registerPointCloud(_name, _mesh.vertex_positions());
    if (_mesh.n_vertex_props()>0) {
        for (auto v_prop = _mesh.vertex_props_begin(); v_prop != _mesh.vertex_props_end(); ++v_prop) {
            if ((*v_prop)->typeNameWrapper() == "double") {
                auto prop = _mesh.get_vertex_property<double>((*v_prop)->name()).value();
                pc->addScalarQuantity((*v_prop)->name(), prop);
            } else if ((*v_prop)->typeNameWrapper() == "int") {
                auto prop = _mesh.get_vertex_property<int>((*v_prop)->name()).value();
                pc->addScalarQuantity((*v_prop)->name(), prop);
            } else if ((*v_prop)->typeNameWrapper() == "vec3d") {
                auto prop = _mesh.get_vertex_property<OpenVolumeMesh::Vec3d>((*v_prop)->name()).value();
                pc->addVectorQuantity((*v_prop)->name(), prop);
            }
        }
    }
    return pc;
}

polyscope::CurveNetwork* Interface::register_curve_network(const std::string& _name, const PolyhedralMesh& _mesh)
{
    if (_mesh.n_edges()==0) {return nullptr;}
    std::vector<std::array<uint32_t,2>> edges;
    edges.reserve(_mesh.n_edges());
    for (auto e_it = _mesh.e_iter(); e_it.is_valid(); ++e_it) {
        const auto& e = _mesh.edge(*e_it);
        edges.push_back({e.from_vertex().uidx(), e.to_vertex().uidx()});
    }
    polyscope::CurveNetwork* cn = polyscope::registerCurveNetwork(_name, _mesh.vertex_positions(), edges);
    return cn;
}

polyscope::SurfaceMesh* Interface::register_surface_mesh(const std::string& _name, const PolyhedralMesh& _mesh)
{
    if (_mesh.n_faces()==0) {return nullptr;}
    std::vector<std::array<uint32_t,3>> tris;
    tris.reserve(_mesh.n_faces());
    for (auto f_it = _mesh.f_iter(); f_it.is_valid(); ++f_it) {
        const auto& vhs = _mesh.get_halfface_vertices(f_it->halfface_handle(0));
        for (uint32_t i = 1; i < vhs.size()-1; ++i) {
            tris.push_back({
                vhs[0].uidx(), vhs[i].uidx(), vhs[i+1].uidx()
            });
        }
    }
    auto* sm = polyscope::registerSurfaceMesh(_name, _mesh.vertex_positions(), tris);
    if (_mesh.n_face_props()>0) {
        for (auto f_prop = _mesh.face_props_begin(); f_prop != _mesh.face_props_begin(); ++f_prop) {
            if ((*f_prop)->typeNameWrapper() == "int") {
                auto prop = _mesh.get_face_property<int>((*f_prop)->name()).value();
                sm->addFaceScalarQuantity((*f_prop)->name(), prop);
            }
        }
    }
    return sm;
}

polyscope::VolumeMesh* Interface::register_volume_mesh(const std::string& _name, const PolyhedralMesh& _mesh)
{
    if (_mesh.n_cells()==0) {return nullptr;}
    if (AxoPlotl::is_tet_mesh(_mesh)) {
        std::vector<std::array<uint32_t,4>> tet_indices;
        tet_indices.reserve(_mesh.n_cells());
        for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {
            int i = 0;
            tet_indices.emplace_back();
            for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
                tet_indices.back()[i++] = cv_it->idx();
            }
        }
        return polyscope::registerTetMesh(_name, _mesh.vertex_positions(), tet_indices);
    } else if (AxoPlotl::is_hex_mesh(_mesh)) {
        std::vector<std::array<uint32_t,8>> hex_indices;
        hex_indices.reserve(_mesh.n_cells());
        for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {
            std::vector<uint32_t> is;
            is.reserve(8);
            for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
                is.push_back(cv_it->uidx());
            }
            hex_indices.push_back({
                is[0], is[1], is[2], is[3],
                is[4], is[7], is[6], is[5]
            });
        }
        return polyscope::registerHexMesh(_name, _mesh.vertex_positions(), hex_indices);
    }
    return nullptr;
}

}
