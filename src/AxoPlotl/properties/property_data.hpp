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
    for (auto vh : _mesh.vertices()) {
        Vec3f p = toVec3<Vec3f>(_mesh.vertex(vh));
        p_attribs.push_back({p,Vec4f(_prop[vh],_prop[vh],_prop[vh],1)});
    }
    _r.updatePointsAttributes(p_attribs);
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
    for (auto eh : _mesh.edges()) {
        auto heh = eh.halfedge_handle(0);
        auto vh0 = _mesh.from_vertex_handle(heh);
        auto vh1 = _mesh.to_vertex_handle(heh);
        Vec3f p0 = toVec3<Vec3f>(_mesh.vertex(vh0));
        Vec3f p1 = toVec3<Vec3f>(_mesh.vertex(vh1));
        l_attribs.push_back({p0, Vec4f(_prop[eh],_prop[eh],_prop[eh],1)});
        l_attribs.push_back({p1, Vec4f(_prop[eh],_prop[eh],_prop[eh],1)});
    }
    _r.updateLinesAttributes(l_attribs);
    _r.settings().useDataForLineColor = false;
};

template<typename T>
std::pair<T,T> get_face_scalar_property_range(
    const VolumeMesh& _mesh,
    const OpenVolumeMesh::PropertyPtr<T,OVM::Entity::Face>& _prop)
{
    if (_mesh.n_edges()==0) {return {0,0};}
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
    for (auto fh : _mesh.faces()) {
        auto hfh = fh.halfface_handle(0);
        for (OVM::VH vh : _mesh.get_halfface_vertices(hfh)) {
            t_attribs.push_back({
                toVec3<Vec3f>(_mesh.vertex(vh)),
                Vec4f(_prop[fh],_prop[fh],_prop[fh],1)
            });
        }
    }
    _r.updateTrianglesAttributes(t_attribs);
    _r.settings().useDataForTriangleColor = false;
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
}

}
