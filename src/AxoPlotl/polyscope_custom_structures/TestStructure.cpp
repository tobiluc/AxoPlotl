#include "TestStructure.h"
#include <Eigen/Core>

// namespace OpenVolumeMesh
// {
// template <> const std::string typeName<Eigen::Matrix3d>() { return "mat3x3d"; }
// template <> const std::string entityTypeName<Eigen::Vector3d>() {return "vec3d";};
// }

namespace polyscope
{

std::string ovm_typename = "OpenVolumeMesh";

OVMStructure::OVMStructure(const std::string& _name, const AxoPlotl::PolyhedralMesh& _mesh)
    :
    QuantityStructure<OVMStructure>(_name, ovm_typename)
{
    std::vector<glm::vec3> positions = standardizeVectorArray<glm::vec3, 3>(_mesh.vertex_positions());

    // Prepare Vertices Point Cloud
    if (_mesh.n_vertices() > 0) {
        vertices_point_cloud_ = std::make_unique<PointCloud>(name, positions);
        vertices_point_cloud_->setPointRadius(0.002f, true);
        vertex_radius_ = vertices_point_cloud_->getPointRadius();

        if (_mesh.n_vertex_props()>0) {
            for (auto v_prop = _mesh.vertex_props_begin(); v_prop != _mesh.vertex_props_end(); ++v_prop) {

                // hack for eigen
                std::string prop_type_name = "";
                if ((*v_prop)->internal_type_name() == typeid(Eigen::Matrix3d).name()) {
                    prop_type_name = "eigen_mat3x3d";
                } else if ((*v_prop)->internal_type_name() == typeid(Eigen::Vector3d).name()) {
                    prop_type_name = "eigen_vec3d";
                } else {
                    prop_type_name = (*v_prop)->typeNameWrapper();
                }

                if (prop_type_name == "bool") {
                    auto prop = _mesh.get_vertex_property<bool>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (prop_type_name == "double") {
                    auto prop = _mesh.get_vertex_property<double>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (prop_type_name == "int") {
                    auto prop = _mesh.get_vertex_property<int>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (prop_type_name == "vec3d") {
                    auto prop = _mesh.get_vertex_property<OpenVolumeMesh::Vec3d>((*v_prop)->name()).value();
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name(), prop);
                } else if (prop_type_name == "eigen_vec3d") {
                    auto prop = _mesh.get_vertex_property<Eigen::Vector3d>((*v_prop)->name()).value();
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name(), prop);
                } else if (prop_type_name == "eigen_mat3x3d") {
                    auto prop = _mesh.get_vertex_property<Eigen::Matrix3d>((*v_prop)->name()).value();
                    std::array<std::vector<Eigen::Vector3d>,3> cols;
                    for (auto v_it = _mesh.v_iter(); v_it.is_valid(); ++v_it) {
                        for (int i = 0; i < 3; ++i) {
                            cols[i].push_back(prop[*v_it].col(i));
                        }
                    }
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name()+" (x)", cols[0]);
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name()+" (y)", cols[1]);
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name()+" (z)", cols[2]);
                } else {
                    std::cerr << "Skipped Property " << (*v_prop)->name() << std::endl;
                }
            }
        }
    }

    // Prepare Edges Curve Network
    if (_mesh.n_edges() > 0) {
        std::vector<std::array<size_t,2>> edges;
        edges.reserve(_mesh.n_edges());
        for (auto e_it = _mesh.e_iter(); e_it.is_valid(); ++e_it) {
            OpenVolumeMesh::HEH heh = e_it->halfedge_handle(0);
            edges.push_back({
                _mesh.from_vertex_handle(heh).uidx(),
                _mesh.to_vertex_handle(heh).uidx()
            });
        }
        edges_curve_network_ = std::make_unique<CurveNetwork>(name, positions, edges);
        edges_curve_network_->setRadius(0.001f, true);
        edge_radius_ = edges_curve_network_->getRadius();

        if (_mesh.n_edge_props()>0) {
            for (auto e_prop = _mesh.edge_props_begin(); e_prop != _mesh.edge_props_end(); ++e_prop) {
                if ((*e_prop)->typeNameWrapper() == "int") {
                    auto prop = _mesh.get_edge_property<int>((*e_prop)->name()).value();
                    edges_curve_network_->addEdgeScalarQuantity((*e_prop)->name(), prop);
                }
            }
        }
    }

    // Prepare Faces Surface Mesh
    if (_mesh.n_faces() > 0) {
        std::vector<std::vector<size_t>> faces;
        faces.reserve(_mesh.n_faces());
        for (auto f_it = _mesh.f_iter(); f_it.is_valid(); ++f_it) {
            OpenVolumeMesh::HFH hfh = f_it->halfface_handle(0);
            faces.emplace_back();
            for (auto hfv_it = _mesh.hfv_iter(hfh); hfv_it.is_valid(); ++hfv_it) {
                faces.back().push_back(hfv_it->uidx());
            }
        }
        faces_surface_mesh_ = std::make_unique<SurfaceMesh>(name, positions, faces);
    }
}

// Build the imgui display
void OVMStructure::buildCustomUI()
{
    //ImGui::Text("Hello World, I am an Open Volume Mesh :)");
    ImGui::Text("#P = %zu, #E = %zu, #F = %zu", n_vertices(), n_edges(), n_faces());

    // if (ImGui::Button("Delete")) {
    //     polyscope::removeStructure(this);
    // }
}

void OVMStructure::buildCustomOptionsUI()
{
    if (n_vertices() && ImGui::SliderFloat("Vertex Radius", &vertex_radius_, 0.f, 0.05f)) {
        vertices_point_cloud_->setPointRadius(vertex_radius_, false);
    }
    if (n_edges() && ImGui::SliderFloat("Edge Radius", &edge_radius_, 0.f, 0.05f)) {
        edges_curve_network_->setRadius(edge_radius_, false);
    }
}

void OVMStructure::buildPickUI(const PickResult& result)
{
}

void OVMStructure::buildQuantitiesUI()
{
    if (faces_surface_mesh_ && faces_surface_mesh_->quantities.size() > 0
        && ImGui::CollapsingHeader("Face Properties")) {
        faces_surface_mesh_->buildQuantitiesUI();
    }
    if (edges_curve_network_ && edges_curve_network_->quantities.size() > 0
        && ImGui::CollapsingHeader("Edge Properties")) {
        edges_curve_network_->buildQuantitiesUI();
    }
    if (vertices_point_cloud_ && vertices_point_cloud_->quantities.size() > 0
        && ImGui::CollapsingHeader("Vertex Properties")) {
        vertices_point_cloud_->buildQuantitiesUI();
    }
}

void OVMStructure::buildStructureOptionsUI()
{

}

// Standard structure overrides
void OVMStructure::draw()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->draw();}
    if (edges_curve_network_) {edges_curve_network_->draw();}
    if (vertices_point_cloud_) {vertices_point_cloud_->draw();}
}

void OVMStructure::drawDelayed()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->drawDelayed();}
    if (edges_curve_network_) {edges_curve_network_->drawDelayed();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawDelayed();}
}

void OVMStructure::drawPick()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->drawPick();}
    if (edges_curve_network_) {edges_curve_network_->drawPick();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawPick();}
}

void OVMStructure::drawPickDelayed()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->drawPickDelayed();}
    if (edges_curve_network_) {edges_curve_network_->drawPickDelayed();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawPickDelayed();}
}

void OVMStructure::updateObjectSpaceBounds()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->updateObjectSpaceBounds();}
    if (edges_curve_network_) {edges_curve_network_->updateObjectSpaceBounds();}
    if (vertices_point_cloud_) {vertices_point_cloud_->updateObjectSpaceBounds();}
}

std::string OVMStructure::typeName()
{
    return ovm_typename;
}

void OVMStructure::refresh()
{
    if (faces_surface_mesh_) {faces_surface_mesh_->refresh();}
    if (edges_curve_network_) {edges_curve_network_->refresh();}
    if (vertices_point_cloud_) {vertices_point_cloud_->refresh();}
}

Structure* OVMStructure::setEnabled(bool newEnabled)
{
    Structure::setEnabled(newEnabled);
    if (vertices_point_cloud_) {vertices_point_cloud_->setEnabled(newEnabled);}
    if (edges_curve_network_) {edges_curve_network_->setEnabled(newEnabled);}
    if (faces_surface_mesh_) {faces_surface_mesh_->setEnabled(newEnabled);}
    return this;
}

OVMStructure* registerOVMStructure(const std::string& name, const AxoPlotl::PolyhedralMesh& _mesh)
{
    auto* s = new polyscope::OVMStructure(name, _mesh);
    bool success = polyscope::registerStructure(s);
    if (!success) {
        safeDelete(s);
        std::cerr << "Failed to register structure " << std::endl;
    }
    return s;
}

}
