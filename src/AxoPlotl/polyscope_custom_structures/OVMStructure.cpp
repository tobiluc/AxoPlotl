#include "OVMStructure.h"
#include <Eigen/Core>

// namespace OpenVolumeMesh
// {
// template <> const std::string typeName<Eigen::Matrix3d>() { return "mat3x3d"; }
// template <> const std::string entityTypeName<Eigen::Vector3d>() {return "vec3d";};
// }

namespace polyscope
{

std::string ovm_typename = "OpenVolumeMesh";

std::string prop_type_name(OpenVolumeMesh::PropertyStorageBase* _p) {
    if (_p->internal_type_name() == typeid(Eigen::Matrix3d).name()) {
        return "eigen_mat3x3d";
    } else if (_p->internal_type_name() == typeid(Eigen::Vector3d).name()) {
        return "eigen_vec3d";
    }
    return _p->typeNameWrapper();
}

OVMStructure::OVMStructure(const std::string& _name, const AxoPlotl::PolyhedralMesh& _mesh)
    :
    QuantityStructure<OVMStructure>(_name, ovm_typename)
{
    std::vector<glm::vec3> points_data_ = standardizeVectorArray<glm::vec3, 3>(_mesh.vertex_positions());

    // Compute Bounding box
    glm::vec3 min = glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();
    glm::vec3 max = -glm::vec3{1, 1, 1} * std::numeric_limits<float>::infinity();
    for (const glm::vec3& p : points_data_) {
        min = componentwiseMin(min, p);
        max = componentwiseMax(max, p);
    }
    objectSpaceBoundingBox = std::make_tuple(min, max);

    // length scale, as twice the radius from the center of the bounding box
    glm::vec3 center = 0.5f * (min + max);
    float lengthScale = 0.0;
    for (const glm::vec3& p : points_data_) {
        lengthScale = std::max(lengthScale, glm::length2(p - center));
    }
    objectSpaceLengthScale = 2 * std::sqrt(lengthScale);

    // Prepare Vertices Point Cloud
    if (_mesh.n_vertices() > 0) {
        vertices_point_cloud_ = std::make_unique<PointCloud>(name, points_data_);
        vertices_point_cloud_->setPointRadius(0.002f, true);
        vertex_radius_ = vertices_point_cloud_->getPointRadius();

        if (_mesh.n_vertex_props()>0) {
            for (auto v_prop = _mesh.vertex_props_begin(); v_prop != _mesh.vertex_props_end(); ++v_prop) {

                // hack for eigen
                std::string type_name = prop_type_name(*v_prop);

                if (type_name == "bool") {
                    auto prop = _mesh.get_vertex_property<bool>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (type_name == "double") {
                    auto prop = _mesh.get_vertex_property<double>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (type_name == "int") {
                    auto prop = _mesh.get_vertex_property<int>((*v_prop)->name()).value();
                    vertices_point_cloud_->addScalarQuantity((*v_prop)->name(), prop);
                } else if (type_name == "vec3d") {
                    auto prop = _mesh.get_vertex_property<OpenVolumeMesh::Vec3d>((*v_prop)->name()).value();
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name(), prop);
                } else if (type_name == "eigen_vec3d") {
                    auto prop = _mesh.get_vertex_property<Eigen::Vector3d>((*v_prop)->name()).value();
                    vertices_point_cloud_->addVectorQuantity((*v_prop)->name(), prop);
                } else if (type_name == "eigen_mat3x3d") {
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
        edges_curve_network_ = std::make_unique<CurveNetwork>(name, points_data_, edges);
        edges_curve_network_->setRadius(0.001f, true);
        edge_radius_ = edges_curve_network_->getRadius();

        if (_mesh.n_edge_props()>0) {
            for (auto e_prop = _mesh.edge_props_begin(); e_prop != _mesh.edge_props_end(); ++e_prop) {
                std::string type_name = prop_type_name(*e_prop);
                if (type_name == "int") {
                    auto prop = _mesh.get_edge_property<int>((*e_prop)->name()).value();
                    edges_curve_network_->addEdgeScalarQuantity((*e_prop)->name(), prop);
                }
            }
        }
    }

    // Prepare Faces Surface Mesh
    if (_mesh.n_faces() > 0) {
        std::vector<std::vector<size_t>> halffaces;
        halffaces.reserve(_mesh.n_halffaces());
        for (auto hf_it = _mesh.hf_iter(); hf_it.is_valid(); ++hf_it) {
            halffaces.emplace_back();
            for (auto hfv_it = _mesh.hfv_iter(*hf_it); hfv_it.is_valid(); ++hfv_it) {
                halffaces.back().push_back(hfv_it->uidx());
            }
        }
        halffaces_surface_mesh_ = std::make_unique<SurfaceMesh>(name, points_data_, halffaces);
        halffaces_surface_mesh_->setBackFacePolicy(BackFacePolicy::Cull);

        if (_mesh.n_halfface_props() > 0) {
            for (auto hf_prop = _mesh.halfface_props_begin(); hf_prop != _mesh.halfface_props_end(); ++hf_prop) {
                std::string type_name = prop_type_name(*hf_prop);
                if (type_name == "bool") {
                    auto prop = _mesh.get_halfface_property<bool>((*hf_prop)->name()).value();
                    halffaces_surface_mesh_->addFaceScalarQuantity((*hf_prop)->name(), prop);
                } else if (type_name == "int") {
                    auto prop = _mesh.get_halfface_property<int>((*hf_prop)->name()).value();
                    halffaces_surface_mesh_->addFaceScalarQuantity((*hf_prop)->name(), prop);
                }
            }
        }
    }

    // Prepare Cell Volume Mesh
    if (_mesh.n_cells() > 0) {

        // For rendering scaled cells, we duplicate the vertices!
        std::vector<glm::vec3> corner_positions;
        corner_positions.reserve(8*_mesh.n_cells());
        std::vector<std::vector<int32_t>> cells;
        cells.reserve(_mesh.n_cells());

        for (auto c_it = _mesh.c_iter(); c_it.is_valid(); ++c_it) {
            cells.emplace_back();
            for (auto cv_it = _mesh.cv_iter(*c_it); cv_it.is_valid(); ++cv_it) {
                const auto& p = _mesh.vertex(*cv_it);
                cells.back().push_back(corner_positions.size());
                corner_positions.emplace_back(p[0], p[1], p[2]);
            }

            // Pad with -1
            while (cells.back().size() < 8) {
                cells.back().push_back(-1);
            }
        }

        cells_volume_mesh_ = std::make_unique<VolumeMesh>(name, corner_positions, standardizeVectorArray<std::array<uint32_t, 8>, 8>(cells));

        if (_mesh.n_cell_props() > 0) {

            for (auto c_prop = _mesh.cell_props_begin(); c_prop != _mesh.cell_props_end(); ++c_prop) {

                std::string type_name = prop_type_name(*c_prop);

                if (type_name == "bool") {
                    auto prop = _mesh.get_cell_property<bool>((*c_prop)->name()).value();
                    cells_volume_mesh_->addCellScalarQuantity((*c_prop)->name(), prop);
                } else if (type_name == "int") {
                    auto prop = _mesh.get_cell_property<int>((*c_prop)->name()).value();
                    cells_volume_mesh_->addCellScalarQuantity((*c_prop)->name(), prop);
                } else if (type_name == "double") {
                    auto prop = _mesh.get_cell_property<double>((*c_prop)->name()).value();
                    cells_volume_mesh_->addCellScalarQuantity((*c_prop)->name(), prop);
                }
            }
        }
    }
}

// Build the imgui display
void OVMStructure::buildCustomUI()
{
    //ImGui::Text("Hello World, I am an Open Volume Mesh :)");
    ImGui::Text("#P = %zu, #E = %zu, #F = %zu, #C = %zu",
        n_vertices(), n_edges(), n_faces(), n_cells());

    if (ImGui::Button("Zoom to Object")) {
        glm::vec3 p = polyscope::view::getCameraWorldPosition();
        glm::vec3 o = 0.5f*(std::get<0>(objectSpaceBoundingBox) + std::get<1>(objectSpaceBoundingBox));
        if (p != o) {
            p = o + objectSpaceLengthScale * glm::normalize(p - o);
            polyscope::view::lookAt(p, o, true);
        }
    }
}

void OVMStructure::buildCustomOptionsUI()
{
    bool b;
    if (n_vertices() && ImGui::CollapsingHeader("Vertex Options")) {
        ImGui::PushID("vertices");
        b = vertices_point_cloud_->isEnabled();
        if (ImGui::Checkbox("Enable", &b)) {
            vertices_point_cloud_->setEnabled(b);
        }
        if (ImGui::SliderFloat("Vertex Radius", &vertex_radius_, 0.f, 0.05f)) {
            vertices_point_cloud_->setPointRadius(vertex_radius_, false);
        }
        ImGui::PopID();
    }
    if (n_edges() && ImGui::CollapsingHeader("Edge Options")) {
        ImGui::PushID("edges");
        b = edges_curve_network_->isEnabled();
        if (ImGui::Checkbox("Enable", &b)) {
            edges_curve_network_->setEnabled(b);
        }
        if (ImGui::SliderFloat("Edge Radius", &edge_radius_, 0.f, 0.05f)) {
            edges_curve_network_->setRadius(edge_radius_, false);
        }
        ImGui::PopID();
    }
    if (n_faces() && ImGui::CollapsingHeader("Face Options")) {
        ImGui::PushID("faces");
        b = halffaces_surface_mesh_->isEnabled();
        if (ImGui::Checkbox("Enable", &b)) {
            halffaces_surface_mesh_->setEnabled(b);
        }
        ImGui::PopID();
    }
    if (n_cells() && ImGui::CollapsingHeader("Cell Options")) {
        ImGui::PushID("cells");
        b = cells_volume_mesh_->isEnabled();
        if (ImGui::Checkbox("Enable", &b)) {
            cells_volume_mesh_->setEnabled(b);
        }
        if (ImGui::SliderFloat("Cell Scale", &cell_scale_, 0.f, 1.0f)) {
            // TODO: We probably want to set some uniform
            // and use a custom shader for cell scales
        }
        ImGui::PopID();
    }
}

void OVMStructure::buildPickUI(const PickResult& result)
{
}

void OVMStructure::buildQuantitiesUI()
{
    if (halffaces_surface_mesh_ && halffaces_surface_mesh_->quantities.size() > 0
        && ImGui::CollapsingHeader("Face Properties")) {
        halffaces_surface_mesh_->buildQuantitiesUI();
    }
    if (edges_curve_network_ && edges_curve_network_->quantities.size() > 0
        && ImGui::CollapsingHeader("Edge Properties")) {
        edges_curve_network_->buildQuantitiesUI();
    }
    if (vertices_point_cloud_ && vertices_point_cloud_->quantities.size() > 0
        && ImGui::CollapsingHeader("Vertex Properties")) {
        vertices_point_cloud_->buildQuantitiesUI();
    }
    if (cells_volume_mesh_ && cells_volume_mesh_->quantities.size() > 0
        && ImGui::CollapsingHeader("Cell Properties")) {
        cells_volume_mesh_->buildQuantitiesUI();
    }
}

void OVMStructure::buildStructureOptionsUI()
{

}

// Standard structure overrides
void OVMStructure::draw()
{
    if (cells_volume_mesh_) {cells_volume_mesh_->draw();}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->draw();}
    if (edges_curve_network_) {edges_curve_network_->draw();}
    if (vertices_point_cloud_) {vertices_point_cloud_->draw();}
}

void OVMStructure::drawDelayed()
{
    if (cells_volume_mesh_) {cells_volume_mesh_->drawDelayed();}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->drawDelayed();}
    if (edges_curve_network_) {edges_curve_network_->drawDelayed();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawDelayed();}
}

void OVMStructure::drawPick()
{
    if (cells_volume_mesh_) {cells_volume_mesh_->drawPick();}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->drawPick();}
    if (edges_curve_network_) {edges_curve_network_->drawPick();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawPick();}
}

void OVMStructure::drawPickDelayed()
{
    if (cells_volume_mesh_) {cells_volume_mesh_->drawPickDelayed();}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->drawPickDelayed();}
    if (edges_curve_network_) {edges_curve_network_->drawPickDelayed();}
    if (vertices_point_cloud_) {vertices_point_cloud_->drawPickDelayed();}
}

void OVMStructure::updateObjectSpaceBounds()
{
    // pass
}

std::string OVMStructure::typeName()
{
    return ovm_typename;
}

void OVMStructure::refresh()
{
    if (cells_volume_mesh_) {cells_volume_mesh_->refresh();}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->refresh();}
    if (edges_curve_network_) {edges_curve_network_->refresh();}
    if (vertices_point_cloud_) {vertices_point_cloud_->refresh();}
}

Structure* OVMStructure::setEnabled(bool newEnabled)
{
    if (newEnabled == isEnabled()) {return this;}
    enabled = newEnabled;
    if (cells_volume_mesh_) {cells_volume_mesh_->setEnabled(newEnabled);}
    if (vertices_point_cloud_) {vertices_point_cloud_->setEnabled(newEnabled);}
    if (edges_curve_network_) {edges_curve_network_->setEnabled(newEnabled);}
    if (halffaces_surface_mesh_) {halffaces_surface_mesh_->setEnabled(newEnabled);}
    return this;
}

OVMStructure* registerOVMStructure(const std::string& name, const AxoPlotl::PolyhedralMesh& _mesh)
{
    checkInitialized();

    auto* s = new polyscope::OVMStructure(name, _mesh);
    bool success = polyscope::registerStructure(s);
    if (!success) {
        safeDelete(s);
        std::cerr << "Failed to register structure " << std::endl;
    }
    return s;
}

}
