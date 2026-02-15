#pragma once

#include "AxoPlotl/properties/property_filters.hpp"
#include "AxoPlotl/rendering/MeshRenderer.h"
#include "AxoPlotl/rendering/VolumeMeshRenderer.hpp"
#include "AxoPlotl/utils/Utils.h"
#include "AxoPlotl/typedefs/typedefs_ToLoG.hpp" // <- important for glm traits
#include "ToLoG/utils/KeyValueVector.hpp"

namespace AxoPlotl
{

template<typename T, typename Entity>
std::pair<T,T> get_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,Entity>& _prop)
{
    if (mesh_n_entities<EntityType<Entity>::type()>(_mesh)==0) {return {0,0};}
    if constexpr(std::is_same_v<T,bool>) {return {false,true};}
    else {
        auto h0 = mesh_entity_handle<EntityType<Entity>::type()>(0);
        std::pair<T,T> r = {_prop[h0], _prop[h0]};
        for (auto h : mesh_entities<EntityType<Entity>::type()>(_mesh)) {
            r.first = std::min(r.first, _prop[h]);
            r.second = std::max(r.second, _prop[h]);
        }
        return r;
    }
};

template<typename T>
static std::string value_to_string(const T& _val)
{
    if constexpr(std::is_same_v<T,bool>) {return _val? "True" : "False";}
    if constexpr(std::is_scalar_v<T>) {return std::string(_val);}
    if constexpr (ToLoG::vector_type<T>) {
        std::string s = "[";
        for (int i = 0; i < ToLoG::Traits<T>::dim-1; ++i) {
            s += std::string(_val[i]) + ", ";
        }
        s += std::string(_val[ToLoG::Traits<T>::dim-1]) + "]";
    }
    return "UNKNOWN";
}

/// Converts a generic value to a Vec4f to store in the Vertex Buffer as v_data.
template<typename T>
Vec4f vertex_buffer_property_data(const T& _val)
{
    if constexpr(std::is_same_v<T,bool>) {return {_val,0,0,1};}
    if constexpr(std::is_same_v<T,int>) {return {_val,0,0,1};}
    if constexpr(std::is_same_v<T,double>) {return {_val,0,0,1};}
    if constexpr(std::is_same_v<T,float>) {return {_val,0,0,1};}
    if constexpr (ToLoG::vector_type<T>) {
        if (ToLoG::Traits<T>::dim <= 4) {
            Vec4f v;
            for (int i = 0; i < ToLoG::Traits<T>::dim; ++i) {
                v[i] = _val[i];
            }
            return v;
        } else {
            std::cerr << "Vector Properties of Dimension > 4 are not supported" << std::endl;
        }
    }
    throw std::runtime_error("UNKNOWN PROPERTY DATA TYPE");
    return Vec4f(0,0,0,0);
}

// template<typename T>
// constexpr GL::MeshRenderer::PropDataType vertex_buffer_property_data_type()
// {
//     if constexpr(std::is_same_v<T,bool>
//                   || std::is_same_v<T,int>
//                   || std::is_same_v<T,float>
//                   || std::is_same_v<T,double>)
//     {return GL::MeshRenderer::PropDataType::SCALAR;}
//     if constexpr(ToLoG::vector_type<T>) {
//         if constexpr(ToLoG::Traits<T>::dim == 3) {
//             return GL::MeshRenderer::PropDataType::VEC3;
//         }
//     }
//     return GL::MeshRenderer::PropDataType::COLOR;
// }

template<typename T>
constexpr VolumeMeshRenderer::Property::Visualization vertex_buffer_property_data_visualization()
{
    if constexpr(std::is_same_v<T,bool>
                  || std::is_same_v<T,int>
                  || std::is_same_v<T,float>
                  || std::is_same_v<T,double>)
    {return VolumeMeshRenderer::Property::Visualization::SCALAR;}
    if constexpr(ToLoG::vector_type<T>) {
        if constexpr(ToLoG::Traits<T>::dim == 3) {
            return VolumeMeshRenderer::Property::Visualization::VEC3;
        }
    }
    return VolumeMeshRenderer::Property::Visualization::COLOR;
}

template<typename T>
void upload_vertex_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Vertex>& _prop,
    VolumeMeshRenderer& _vol_rend)
{
    if (_mesh.n_vertices()==0) {return;}
    std::vector<VolumeMeshRenderer::VertexData> v_data;
    for (auto vh : _mesh.vertices()) {
        Vec4f a = vertex_buffer_property_data(_prop[vh]);
        v_data.push_back({.property = a});
    }
    _vol_rend.update_vertex_data(v_data);
    _vol_rend.vertex_property_.vis_ = vertex_buffer_property_data_visualization<T>();

    // std::vector<GL::MeshRenderer::VertexPointAttrib> p_attribs;
    // p_attribs.reserve(_mesh.n_vertices());
    // std::vector<GLuint> indices;
    // indices.reserve(_mesh.n_vertices());
    // for (auto vh : _mesh.vertices()) {
    //     Vec3f p = toVec3<Vec3f>(_mesh.vertex(vh));
    //     Vec4f a = vertex_buffer_property_data(_prop[vh]);
    //     p_attribs.push_back({
    //         .position = p,
    //         .color = a
    //     });
    //     indices.push_back(vh.uidx());
    // }
    // _r.updateVertexPoints(p_attribs, indices);
    // _r.vertex_prop_type_ = vertex_buffer_property_data_type<T>();
    std::cout << "Uploaded Vertex Property Data " << (int)_vol_rend.edge_property_.vis_ << std::endl;
};

template<typename T>
void upload_edge_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Edge>& _prop,
    VolumeMeshRenderer& _vol_rend)
{
    if (_mesh.n_edges()==0) {return;}
    std::vector<VolumeMeshRenderer::EdgeData> e_data;
    for (auto eh : _mesh.edges()) {
        Vec4f a = vertex_buffer_property_data(_prop[eh]);
        e_data.push_back({.property = a});
    }
    _vol_rend.update_edge_data(e_data);
    _vol_rend.edge_property_.vis_ = vertex_buffer_property_data_visualization<T>();

    // std::vector<GL::MeshRenderer::VertexLineAttrib> l_attribs;
    // l_attribs.reserve(2*_mesh.n_edges());
    // std::vector<GLuint> indices;
    // indices.reserve(2*_mesh.n_edges());
    // for (auto eh : _mesh.edges()) {
    //     auto heh = eh.halfedge_handle(0);
    //     auto vh0 = _mesh.from_vertex_handle(heh);
    //     auto vh1 = _mesh.to_vertex_handle(heh);
    //     Vec3f p0 = toVec3<Vec3f>(_mesh.vertex(vh0));
    //     Vec3f p1 = toVec3<Vec3f>(_mesh.vertex(vh1));
    //     l_attribs.push_back({
    //         .position = p0,
    //         .color = vertex_buffer_property_data(_prop[eh])
    //     });
    //     l_attribs.push_back({
    //         .position = p1,
    //         .color = vertex_buffer_property_data(_prop[eh])
    //     });
    //     indices.push_back(l_attribs.size()-2);
    //     indices.push_back(l_attribs.size()-1);
    // }
    // _r.updateEdgeLines(l_attribs, indices);
    // _r.edge_prop_type_ = vertex_buffer_property_data_type<T>();
    std::cout << "Uploaded Edge Property Data " << (int)_vol_rend.edge_property_.vis_ << std::endl;
};

template<typename T>
void upload_face_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Face>& _prop,
    VolumeMeshRenderer& _vol_rend)
{
    if (_mesh.n_faces()==0) {return;}
    std::vector<VolumeMeshRenderer::FaceData> f_data;
    for (auto fh : _mesh.faces()) {
        Vec4f a = vertex_buffer_property_data(_prop[fh]);
        f_data.push_back({.property = a});
    }
    _vol_rend.update_face_data(f_data);
    _vol_rend.face_property_.vis_ = vertex_buffer_property_data_visualization<T>();

    // std::vector<GL::MeshRenderer::VertexTriangleAttrib> t_attribs;
    // t_attribs.reserve(3*_mesh.n_faces());
    // std::vector<GLuint> indices;
    // indices.reserve(3*_mesh.n_faces());
    // size_t idx_offset(0);
    // for (auto fh : _mesh.faces()) {
    //     auto hfh = fh.halfface_handle(0);
    //     const auto& vhs = _mesh.get_halfface_vertices(hfh);
    //     for (OVM::VH vh : _mesh.get_halfface_vertices(hfh)) {
    //         t_attribs.push_back({
    //             .position = toVec3<Vec3f>(_mesh.vertex(vh)),
    //             .color = vertex_buffer_property_data(_prop[fh]),
    //             .face_index = static_cast<float>(fh.idx())
    //         });
    //     }
    //     for (uint j = 1; j < vhs.size()-1; ++j) {
    //         indices.push_back(idx_offset);
    //         indices.push_back(idx_offset+j);
    //         indices.push_back(idx_offset+j+1);
    //     }
    //     idx_offset += vhs.size();
    // }
    // _r.updateFaceTriangles(t_attribs, indices);
    // _r.face_prop_type_ = vertex_buffer_property_data_type<T>();
    std::cout << "Uploaded Face Property Data " << (int)_vol_rend.edge_property_.vis_ << std::endl;
};

template<typename T>
void upload_cell_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Cell>& _prop,
    VolumeMeshRenderer& _vol_rend)
{
    if (_mesh.n_cells()==0) {return;}
    std::vector<VolumeMeshRenderer::CellData> c_data;
    for (auto ch : _mesh.cells()) {
        Vec4f a = vertex_buffer_property_data(_prop[ch]);
        c_data.push_back({.property = a});
    }
    _vol_rend.update_cell_data(c_data);
    _vol_rend.cell_property_.vis_ = vertex_buffer_property_data_visualization<T>();

    // std::vector<GL::MeshRenderer::VertexCellAttrib> c_attribs;
    // c_attribs.reserve(4*_mesh.n_cells());
    // std::vector<GLuint> c_triangle_indices;
    // c_triangle_indices.reserve(12*_mesh.n_cells());
    // std::vector<GLuint> c_line_indices;
    // c_line_indices.reserve(12*_mesh.n_cells());
    // unsigned int v_index(0);

    // ToLoG::KeyValueVector<OVM::VH,int> vh_idx;
    // for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {

    //     // Compute Incenter
    //     uint32_t n_vhs(0);
    //     Vec3f incenter(0,0,0);
    //     for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
    //         incenter += toVec3<Vec3f>(_mesh.vertex(*cv_it));
    //         ++n_vhs;
    //     }
    //     incenter /= n_vhs; // todo. compute actual incenter

    //     // Collect the cell vertices and create a mapping
    //     // vh -> idx

    //     //ToLoG::HashMap<OVM::VH,int> ;
    //     vh_idx.clear();
    //     for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
    //         OpenVolumeMesh::VH vh = *cv_it;
    //         vh_idx[vh] = v_index++;

    //         auto p = toVec3<Vec3f>(_mesh.vertex(vh));
    //         c_attribs.push_back(GL::MeshRenderer::VertexCellAttrib{
    //             .position = p,
    //             .data = vertex_buffer_property_data(_prop[*c_it]),
    //             .cell_incenter = incenter,
    //             .cell_index = static_cast<float>(c_it->idx())
    //         });
    //     }

    //     // Add the cell edges
    //     for (auto ce_it = _mesh.ce_iter(*c_it); ce_it.is_valid(); ++ce_it) {
    //         auto heh = ce_it->halfedge_handle(0);
    //         c_line_indices.push_back(vh_idx[_mesh.from_vertex_handle(heh)]);
    //         c_line_indices.push_back(vh_idx[_mesh.to_vertex_handle(heh)]);
    //     }

    //     // Triangulate the cell halffaces
    //     for (const auto& hfh : _mesh.cell(*c_it).halffaces()) {
    //         const auto& vhs = _mesh.get_halfface_vertices(hfh);

    //         for (uint j = 1; j < vhs.size()-1; ++j) {
    //             c_triangle_indices.push_back(vh_idx[vhs[0]]);
    //             c_triangle_indices.push_back(vh_idx[vhs[j]]);
    //             c_triangle_indices.push_back(vh_idx[vhs[j+1]]);
    //         }

    //     }
    // }
    // _r.updateCellTriangles(c_attribs, c_triangle_indices, c_line_indices);
    // _r.cell_prop_type_ = vertex_buffer_property_data_type<T>();
    std::cout << "Uploaded Cell Property Data " << (int)_vol_rend.edge_property_.vis_ << std::endl;
};

template<typename T, typename Entity>
void upload_property_data(
    const VolumeMesh& _mesh,
    OpenVolumeMesh::PropertyStorageBase* _prop,
    std::vector<std::shared_ptr<PropertyFilterBase>>& _prop_filters,
    VolumeMeshRenderer& _vol_rend
    )
{
    _prop_filters.clear();
    auto prop = _mesh.get_property<T,Entity>((_prop)->name()).value();

    // Setup Property Filters
    if constexpr(std::is_same_v<T,bool>
            || std::is_same_v<T,int>
            || std::is_same_v<T,float>
            || std::is_same_v<T,double>)
    {
        auto r = get_scalar_property_range<T,Entity>(_mesh, prop);
        _prop_filters.push_back(std::make_shared<ScalarPropertyRangeFilter<T,Entity>>(r.first, r.second));
        _prop_filters.push_back(std::make_shared<ScalarPropertyExactFilter<T,Entity>>());
    }

    // Upload Properties
    if constexpr(std::is_same_v<Entity,OVM::Entity::Vertex>) {
        upload_vertex_property_data(_mesh, prop, _vol_rend);
        _vol_rend.render_vertices_ = true;
        _vol_rend.render_edges_ = false;
        _vol_rend.render_faces_ = false;
        _vol_rend.render_cells_ = false;
    }
    if constexpr(std::is_same_v<Entity,OVM::Entity::Edge>) {
        upload_edge_property_data(_mesh, prop, _vol_rend);
        _vol_rend.render_vertices_ = false;
        _vol_rend.render_edges_ = true;
        _vol_rend.render_faces_ = false;
        _vol_rend.render_cells_ = false;
    }
    if constexpr(std::is_same_v<Entity,OVM::Entity::Face>) {
        upload_face_property_data(_mesh, prop, _vol_rend);
        _vol_rend.render_vertices_ = false;
        _vol_rend.render_edges_ = false;
        _vol_rend.render_faces_ = true;
        _vol_rend.render_cells_ = false;
    }
    if constexpr(std::is_same_v<Entity,OVM::Entity::Cell>) {
        upload_cell_property_data(_mesh, prop, _vol_rend);
        _vol_rend.render_vertices_ = false;
        _vol_rend.render_edges_ = false;
        _vol_rend.render_faces_ = false;
        _vol_rend.render_cells_ = true;
    }
}

}
