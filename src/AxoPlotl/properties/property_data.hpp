#pragma once

#include "AxoPlotl/properties/property_filters.hpp"
#include "AxoPlotl/rendering/MeshRenderer.h"
#include "AxoPlotl/utils/Utils.h"


namespace AxoPlotl
{

template<typename T>
std::pair<T,T> get_vertex_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Vertex>& _prop)
{
    if (_mesh.n_vertices()==0) {return {0,0};}
    if constexpr(std::is_same_v<T,bool>) {return {false,true};}
    std::pair<T,T> r = {_prop[OpenVolumeMesh::VH(0)], _prop[OpenVolumeMesh::VH(0)]};
    for (auto vh : _mesh.vertices()) {
        r.first = std::min(r.first, _prop[vh]);
        r.second = std::max(r.second, _prop[vh]);
    }
    return r;
};

template<typename T>
void upload_vertex_scalar_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Vertex>& _prop,
    GL::MeshRenderer& _r)
{
    if (_mesh.n_vertices()==0) {return;}
    std::vector<GL::MeshRenderer::VertexPointAttrib> p_attribs;
    p_attribs.reserve(_mesh.n_vertices());
    std::vector<GLuint> indices;
    indices.reserve(_mesh.n_vertices());
    for (auto vh : _mesh.vertices()) {
        Vec3f p = toVec3<Vec3f>(_mesh.vertex(vh));
        p_attribs.push_back({
            .position = p,
            .color = Vec4f(_prop[vh],_prop[vh],_prop[vh],1)
        });
        indices.push_back(vh.idx());
    }
    _r.updateVertexPoints(p_attribs, indices);
    _r.settings().useDataForPointColor = false;
};

template<typename T>
std::pair<T,T> get_edge_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Edge>& _prop)
{
    if (_mesh.n_edges()==0) {return {0,0};}
    if constexpr(std::is_same_v<T,bool>) {return {false,true};}
    std::pair<T,T> r = {_prop[OVM::EH(0)], _prop[OVM::EH(0)]};
    for (auto eh : _mesh.edges()) {
        r.first = std::min(r.first, _prop[eh]);
        r.second = std::max(r.second, _prop[eh]);
    }
    return r;
};

template<typename T>
void upload_edge_scalar_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Edge>& _prop,
    GL::MeshRenderer& _r)
{
    if (_mesh.n_edges()==0) {return;}
    std::vector<GL::MeshRenderer::VertexLineAttrib> l_attribs;
    l_attribs.reserve(2*_mesh.n_edges());
    std::vector<GLuint> indices;
    indices.reserve(2*_mesh.n_edges());
    for (auto eh : _mesh.edges()) {
        auto heh = eh.halfedge_handle(0);
        auto vh0 = _mesh.from_vertex_handle(heh);
        auto vh1 = _mesh.to_vertex_handle(heh);
        Vec3f p0 = toVec3<Vec3f>(_mesh.vertex(vh0));
        Vec3f p1 = toVec3<Vec3f>(_mesh.vertex(vh1));
        l_attribs.push_back({
            .position = p0,
            .color = Vec4f(_prop[eh],_prop[eh],_prop[eh],1)
        });
        l_attribs.push_back({
            .position = p1,
            .color = Vec4f(_prop[eh],_prop[eh],_prop[eh],1)
        });
        indices.push_back(l_attribs.size()-2);
        indices.push_back(l_attribs.size()-1);
    }
    _r.updateEdgeLines(l_attribs, indices);
    _r.settings().useDataForLineColor = false;
};

template<typename T>
std::pair<T,T> get_face_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OVM::Entity::Face>& _prop)
{
    if (_mesh.n_faces()==0) {return {0,0};}
    if constexpr(std::is_same_v<T,bool>) {return {false,true};}
    std::pair<T,T> r = {std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};
    for (auto fh : _mesh.faces()) {
        r.first = std::min(r.first, _prop[fh]);
        r.second = std::max(r.second, _prop[fh]);
    }
    return r;
};

template<typename T>
void upload_face_scalar_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Face>& _prop,
    GL::MeshRenderer& _r)
{
    if (_mesh.n_faces()==0) {return;}
    std::vector<GL::MeshRenderer::VertexTriangleAttrib> t_attribs;
    t_attribs.reserve(3*_mesh.n_faces());
    std::vector<GLuint> indices;
    indices.reserve(3*_mesh.n_faces());
    size_t idx_offset(0);
    for (auto fh : _mesh.faces()) {
        auto hfh = fh.halfface_handle(0);
        auto normal = toVec3<Vec3f>(_mesh.normal(hfh));
        const auto& vhs = _mesh.get_halfface_vertices(hfh);
        for (OVM::VH vh : _mesh.get_halfface_vertices(hfh)) {
            t_attribs.push_back({
                .position = toVec3<Vec3f>(_mesh.vertex(vh)),
                .color = Vec4f(_prop[fh],_prop[fh],_prop[fh],1),
                .normal = normal,
                .face_index = static_cast<float>(fh.idx())
            });
        }
        for (uint j = 1; j < vhs.size()-1; ++j) {
            indices.push_back(idx_offset);
            indices.push_back(idx_offset+j);
            indices.push_back(idx_offset+j+1);
        }
        idx_offset += vhs.size();
    }
    _r.updateFaceTriangles(t_attribs, indices);
    _r.settings().useDataForTriangleColor = false;
};

template<typename T>
std::pair<T,T> get_cell_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OVM::Entity::Cell>& _prop)
{
    if (_mesh.n_cells()==0) {return {0,0};}
    if constexpr(std::is_same_v<T,bool>) {return {false,true};}
    std::pair<T,T> r = {std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};
    for (auto ch : _mesh.cells()) {
        r.first = std::min(r.first, _prop[ch]);
        r.second = std::max(r.second, _prop[ch]);
    }
    return r;
};

template<typename T>
void upload_cell_scalar_property_data(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OpenVolumeMesh::Entity::Cell>& _prop,
    GL::MeshRenderer& _r)
{
    if (_mesh.n_cells()==0) {return;}
    std::vector<GL::MeshRenderer::VertexCellAttrib> c_attribs;
    c_attribs.reserve(4*_mesh.n_cells());
    std::vector<GLuint> c_triangle_indices;
    c_triangle_indices.reserve(12*_mesh.n_cells());
    int idx_offset(0);

    for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {

        uint32_t n_vhs(0);
        Vec3f incenter(0,0,0);
        for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
            incenter += toVec3<Vec3f>(_mesh.vertex(*cv_it));
            ++n_vhs;
        }
        incenter /= n_vhs; // todo. compute actual incenter

        std::vector<OpenVolumeMesh::VH> cell_vhs;
        for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
            OpenVolumeMesh::VH vh = *cv_it;
            cell_vhs.push_back(vh);

            auto p = toVec3<Vec3f>(_mesh.vertex(vh));
            c_attribs.push_back(GL::MeshRenderer::VertexCellAttrib{
                .position = p,
                .data = Vec4f(_prop[*c_it],_prop[*c_it],_prop[*c_it],1),
                .cell_incenter = incenter,
                .cell_index = static_cast<float>(c_it->idx())
            });
        }

        for (const auto& hfh : _mesh.cell(*c_it).halffaces()) {
            const auto& vhs = _mesh.get_halfface_vertices(hfh);
            std::vector<int> vhs_inds;
            for (const auto& vh : vhs) {
                for (int vh_idx = 0; vh_idx < cell_vhs.size(); ++vh_idx) {
                    if (cell_vhs[vh_idx] == vh) {
                        vhs_inds.push_back(vh_idx);
                        break;
                    }
                }
            }

            for (uint j = 1; j < vhs.size()-1; ++j) {
                c_triangle_indices.push_back(idx_offset + vhs_inds[0]);
                c_triangle_indices.push_back(idx_offset + vhs_inds[j]);
                c_triangle_indices.push_back(idx_offset + vhs_inds[j+1]);
            }

        }

        idx_offset += cell_vhs.size();
    }
    _r.updateCellTriangles(c_attribs, c_triangle_indices);
    _r.settings().use_data_as_cell_color_ = false;
};

void upload_property_data(
    const VolumeMesh& mesh_,
    OpenVolumeMesh::PropertyStorageBase* _prop,
    std::shared_ptr<PropertyFilterBase>& prop_filter,
    GL::MeshRenderer& _r)
{
    if (_prop->entity_type() == OVM::EntityType::Vertex)
    {
        if ((_prop)->typeNameWrapper()=="double") {
            auto r = get_vertex_scalar_property_range(mesh_, mesh_.get_vertex_property<double>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<double>>(r.first, r.second);
            upload_vertex_scalar_property_data(mesh_, mesh_.get_vertex_property<double>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="int") {
            //auto r = get_vertex_scalar_property_range(mesh_, mesh_.get_vertex_property<int>((*prop_)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyExactFilter<int>>();
            upload_vertex_scalar_property_data(mesh_, mesh_.get_vertex_property<int>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="float") {
            auto r = get_vertex_scalar_property_range(mesh_, mesh_.get_vertex_property<float>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<float>>(r.first, r.second);
            upload_vertex_scalar_property_data(mesh_, mesh_.get_vertex_property<float>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="bool") {
            prop_filter = std::make_shared<ScalarPropertyExactFilter<bool>>();
            upload_vertex_scalar_property_data(mesh_, mesh_.get_vertex_property<bool>((_prop)->name()).value(), _r);
        }
    }
    else if (_prop->entity_type() == OVM::EntityType::Edge)
    {
        if ((_prop)->typeNameWrapper()=="double") {
            auto r = get_edge_scalar_property_range(mesh_, mesh_.get_edge_property<double>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<double>>(r.first, r.second);
            upload_edge_scalar_property_data(mesh_, mesh_.get_edge_property<double>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="int") {
            //auto r = get_edge_scalar_property_range(mesh_, mesh_.get_edge_property<int>((*prop_)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyExactFilter<int>>();
            upload_edge_scalar_property_data(mesh_, mesh_.get_edge_property<int>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="float") {
            auto r = get_edge_scalar_property_range(mesh_, mesh_.get_edge_property<float>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<float>>(r.first, r.second);
            upload_edge_scalar_property_data(mesh_, mesh_.get_edge_property<float>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="bool") {
            prop_filter = std::make_shared<ScalarPropertyExactFilter<bool>>();
            upload_edge_scalar_property_data(mesh_, mesh_.get_edge_property<bool>((_prop)->name()).value(), _r);
        }
    }
    else if (_prop->entity_type() == OVM::EntityType::Face)
    {
        if ((_prop)->typeNameWrapper()=="double") {
            auto r = get_face_scalar_property_range(mesh_, mesh_.get_face_property<double>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<double>>(r.first, r.second);
            upload_face_scalar_property_data(mesh_, mesh_.get_face_property<double>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="int") {
            //auto r = get_vertex_scalar_property_range(mesh_, mesh_.get_vertex_property<int>((*prop_)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyExactFilter<int>>();
            upload_face_scalar_property_data(mesh_, mesh_.get_face_property<int>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="float") {
            auto r = get_face_scalar_property_range(mesh_, mesh_.get_face_property<float>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<float>>(r.first, r.second);
            upload_face_scalar_property_data(mesh_, mesh_.get_face_property<float>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="bool") {
            prop_filter = std::make_shared<ScalarPropertyExactFilter<bool>>();
            upload_face_scalar_property_data(mesh_, mesh_.get_face_property<bool>((_prop)->name()).value(), _r);
        }
    }
    else if (_prop->entity_type() == OVM::EntityType::Cell)
    {
        if ((_prop)->typeNameWrapper()=="double") {
            auto r = get_cell_scalar_property_range(mesh_, mesh_.get_cell_property<double>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<double>>(r.first, r.second);
            upload_cell_scalar_property_data(mesh_, mesh_.get_cell_property<double>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="int") {
            //auto r = get_vertex_scalar_property_range(mesh_, mesh_.get_vertex_property<int>((*prop_)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyExactFilter<int>>();
            upload_cell_scalar_property_data(mesh_, mesh_.get_cell_property<int>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="float") {
            auto r = get_cell_scalar_property_range(mesh_, mesh_.get_cell_property<float>((_prop)->name()).value());
            prop_filter = std::make_shared<ScalarPropertyRangeFilter<float>>(r.first, r.second);
            upload_cell_scalar_property_data(mesh_, mesh_.get_cell_property<float>((_prop)->name()).value(), _r);
        } else if ((_prop)->typeNameWrapper()=="bool") {
            prop_filter = std::make_shared<ScalarPropertyExactFilter<bool>>();
            upload_cell_scalar_property_data(mesh_, mesh_.get_cell_property<bool>((_prop)->name()).value(), _r);
        }
    }
    else
    {
        std::cerr << "Unsupported entity type for property visualization" << std::endl;
    }
}

}
