#include "MeshNode.h"
#include "AxoPlotl/properties/property_data.hpp"
#include "AxoPlotl/rendering/VolumeMeshRenderer.hpp"
#include "AxoPlotl/utils/string_format.hpp"

namespace AxoPlotl
{

void MeshNode::init(Scene* scene)
{
    VolumeMeshRenderer::StaticData data;
    data.n_vertices_ = mesh_.n_vertices();
    for (auto vh : mesh_.vertices()) {
        const auto& p = mesh_.vertex(vh);
        data.positions_.push_back(Vec4f(p[0],p[1],p[2],1));
        data.vertex_draw_vertices_.push_back(vh.uidx());
    }
    data.n_edges_ = mesh_.n_edges();
    for (auto eh : mesh_.edges()) {
        auto vh0 = mesh_.edge(eh).from_vertex();
        auto vh1 = mesh_.edge(eh).to_vertex();
        data.edge_draw_vertices_.push_back({.vertex_index = vh0.uidx(), .edge_index = eh.uidx()});
        data.edge_draw_vertices_.push_back({.vertex_index = vh1.uidx(), .edge_index = eh.uidx()});
    }
    data.n_faces_ = mesh_.n_faces();
    for (auto fh : mesh_.faces()) {
        const auto& vhs = mesh_.get_halfface_vertices(fh.halfface_handle(0));
        for (int i = 1; i < vhs.size()-1; ++i) {
            data.face_triangle_draw_vertices_.push_back({
                .vertex_index = vhs[0].uidx(),
                .face_index = fh.uidx()
            });
            data.face_triangle_draw_vertices_.push_back({
                .vertex_index = vhs[i].uidx(),
                .face_index = fh.uidx()
            });
            data.face_triangle_draw_vertices_.push_back({
                .vertex_index = vhs[i+1].uidx(),
                .face_index = fh.uidx()
            });
        }
    }
    data.n_cells_ = mesh_.n_cells();
    for (auto ch : mesh_.cells()) {
        // Incenter
        Vec4f incenter(0,0,0,0);
        uint32_t n_vhs(0);
        for (auto cv_it = mesh_.cv_iter(ch); cv_it.is_valid(); ++cv_it) {
            const auto& p = mesh_.vertex(*cv_it);
            incenter += Vec4f(p[0],p[1],p[2],0);
            ++n_vhs;
        }
        incenter /= n_vhs;
        incenter[3] = 1;
        // triangulate each face
        for (auto hfh : mesh_.cell(ch).halffaces()) {
            const auto& vhs = mesh_.get_halfface_vertices(hfh);
            for (int i = 1; i < vhs.size()-1; ++i) {
                data.cell_triangle_draw_vertices_.push_back({
                    .vertex_index = vhs[0].uidx(),
                    .cell_index = ch.uidx(),
                    .incenter = incenter
                });
                data.cell_triangle_draw_vertices_.push_back({
                    .vertex_index = vhs[i].uidx(),
                    .cell_index = ch.uidx(),
                    .incenter = incenter
                });
                data.cell_triangle_draw_vertices_.push_back({
                    .vertex_index = vhs[i+1].uidx(),
                    .cell_index = ch.uidx(),
                    .incenter = incenter
                });
            }
        }
        // outline
        for (auto ce_it = mesh_.ce_iter(ch); ce_it.is_valid(); ++ce_it) {
            auto vh0 = mesh_.edge(*ce_it).from_vertex();
            auto vh1 = mesh_.edge(*ce_it).to_vertex();
            data.cell_line_draw_vertices_.push_back({
                .vertex_index = vh0.uidx(),
                .cell_index = ch.uidx(),
                .incenter = incenter
            });
            data.cell_line_draw_vertices_.push_back({
                .vertex_index = vh1.uidx(),
                .cell_index = ch.uidx(),
                .incenter = incenter
            });
        }
    }
    vol_rend_.init(data);

    // Color the mesh for the gpu by creating private color properties.
    auto v_prop = mesh_.create_private_vertex_property<Vec4f>();
    for (auto vh : mesh_.vertices()) {v_prop[vh] = color_on_sphere(toVec3<Vec3f>(mesh_.vertex(vh)));}
    upload_vertex_property_data(mesh_, v_prop,  vol_rend_);

    auto e_prop = mesh_.create_private_edge_property<Vec4f>();
    for (auto eh : mesh_.edges()) {e_prop[eh] = Vec4f(0,0,0,1);}
    upload_edge_property_data(mesh_, e_prop,  vol_rend_);

    auto f_prop = mesh_.create_private_face_property<Vec4f>();
    for (auto fh : mesh_.faces()) {
        f_prop[fh] = color_on_sphere(toVec3<Vec3f>(
            mesh_.vertex(mesh_.get_halfface_vertices(fh.halfface_handle(0)).at(0))));
    }
    upload_face_property_data(mesh_, f_prop,  vol_rend_);

    auto c_prop = mesh_.create_private_cell_property<Vec4f>();
    for (auto ch : mesh_.cells()) {
        c_prop[ch] = color_on_sphere(toVec3<Vec3f>(
            mesh_.vertex(mesh_.get_halfface_vertices(mesh_.cell(ch).halffaces().at(0)).at(0))));
    }
    upload_cell_property_data(mesh_, c_prop, vol_rend_);

    // Compute Bounding Box
    bbox_ = {Vec3f(std::numeric_limits<float>::infinity()), Vec3f(-std::numeric_limits<float>::infinity())};
    for (auto vh : mesh_.vertices()) {
        const auto& p = mesh_.vertex(vh);
        for (int a = 0; a < 3; ++a) {
            bbox_.first[a] = std::min(bbox_.first[a], p[a]);
            bbox_.second[a] = std::max(bbox_.second[a], p[a]);
        }
    }
}

void MeshNode::renderUIBody(Scene* scene)
{
    ImGui::SeparatorText("Properties");

    if (ImGui::BeginMenu("Calculate")) {
        if (mesh_.n_cells()>0 && ImGui::MenuItem("Cell Boundary Distance")) {
            calc_cell_boundary_distance(mesh_);
        }
        if (mesh_.n_cells()>0 && ImGui::MenuItem("Cell Min Dihedral Angle")) {
            calc_cell_min_dihedral_angle(mesh_);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Select"))
    {
        ImGui::Separator();

        if (mesh_.n_vertex_props()>0 && ImGui::BeginMenu("Vertices"))
        {
            for (auto v_prop = mesh_.vertex_props_begin(); v_prop != mesh_.vertex_props_end(); ++v_prop) {
                ImGui::PushID((*v_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*v_prop)->name().c_str(), (*v_prop)->typeNameWrapper().c_str()).c_str())) {
                    prop_ = *v_prop;
                    if ((*v_prop)->typeNameWrapper()=="double") {
                        upload_property_data<double,OVM::Entity::Vertex>(mesh_, *v_prop, prop_filters_,  vol_rend_);
                    } else if ((*v_prop)->typeNameWrapper()=="int") {
                        upload_property_data<int,OVM::Entity::Vertex>(mesh_, *v_prop, prop_filters_,  vol_rend_);
                    } else if ((*v_prop)->typeNameWrapper()=="float") {
                        upload_property_data<float,OVM::Entity::Vertex>(mesh_, *v_prop, prop_filters_,  vol_rend_);
                    } else if ((*v_prop)->typeNameWrapper()=="bool") {
                        upload_property_data<bool,OVM::Entity::Vertex>(mesh_, *v_prop, prop_filters_,  vol_rend_);
                    } else if ((*v_prop)->typeNameWrapper()=="vec3d") {
                        upload_property_data<OVM::Vec3d,OVM::Entity::Vertex>(mesh_, *v_prop, prop_filters_,  vol_rend_);
                    }
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_edge_props()>0 && ImGui::BeginMenu("Edges"))
        {
            for (auto e_prop = mesh_.edge_props_begin(); e_prop != mesh_.edge_props_end(); ++e_prop) {
                ImGui::PushID((*e_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*e_prop)->name().c_str(), (*e_prop)->typeNameWrapper().c_str()).c_str())) {
                    prop_ = *e_prop;
                    if ((*e_prop)->typeNameWrapper()=="double") {
                        upload_property_data<double,OVM::Entity::Edge>(mesh_, *e_prop, prop_filters_,  vol_rend_);
                    } else if ((*e_prop)->typeNameWrapper()=="int") {
                        upload_property_data<int,OVM::Entity::Edge>(mesh_, *e_prop, prop_filters_,  vol_rend_);
                    } else if ((*e_prop)->typeNameWrapper()=="float") {
                        upload_property_data<float,OVM::Entity::Edge>(mesh_, *e_prop, prop_filters_,  vol_rend_);
                    } else if ((*e_prop)->typeNameWrapper()=="bool") {
                        upload_property_data<bool,OVM::Entity::Edge>(mesh_, *e_prop, prop_filters_,  vol_rend_);
                    } else if ((*e_prop)->typeNameWrapper()=="vec3d") {
                        upload_property_data<OVM::Vec3d,OVM::Entity::Edge>(mesh_, *e_prop, prop_filters_,  vol_rend_);
                    }
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Edge Props
        }

        if (mesh_.n_face_props()>0 && ImGui::BeginMenu("Faces"))
        {
            for (auto f_prop = mesh_.face_props_begin(); f_prop != mesh_.face_props_end(); ++f_prop) {
                ImGui::PushID((*f_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*f_prop)->name().c_str(), (*f_prop)->typeNameWrapper().c_str()).c_str())) {
                    prop_ = *f_prop;
                    if ((*f_prop)->typeNameWrapper()=="double") {
                        upload_property_data<double,OVM::Entity::Face>(mesh_, *f_prop, prop_filters_,  vol_rend_);
                    } else if ((*f_prop)->typeNameWrapper()=="int") {
                        upload_property_data<int,OVM::Entity::Face>(mesh_, *f_prop, prop_filters_,  vol_rend_);
                    } else if ((*f_prop)->typeNameWrapper()=="float") {
                        upload_property_data<float,OVM::Entity::Face>(mesh_, *f_prop, prop_filters_,  vol_rend_);
                    } else if ((*f_prop)->typeNameWrapper()=="bool") {
                        upload_property_data<bool,OVM::Entity::Face>(mesh_, *f_prop, prop_filters_,  vol_rend_);
                    } else if ((*f_prop)->typeNameWrapper()=="vec3d") {
                        upload_property_data<OVM::Vec3d,OVM::Entity::Face>(mesh_, *f_prop, prop_filters_,  vol_rend_);
                    }
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Face Props
        }

        if (mesh_.n_cell_props()>0 && ImGui::BeginMenu("Cells"))
        {
            for (auto c_prop = mesh_.cell_props_begin(); c_prop != mesh_.cell_props_end(); ++c_prop) {
                ImGui::PushID((*c_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*c_prop)->name().c_str(), (*c_prop)->typeNameWrapper().c_str()).c_str())) {
                    prop_ = *c_prop;
                    if ((*c_prop)->typeNameWrapper()=="double") {
                        upload_property_data<double,OVM::Entity::Cell>(mesh_, *c_prop, prop_filters_,  vol_rend_);
                    } else if ((*c_prop)->typeNameWrapper()=="int") {
                        upload_property_data<int,OVM::Entity::Cell>(mesh_, *c_prop, prop_filters_,  vol_rend_);
                    } else if ((*c_prop)->typeNameWrapper()=="float") {
                        upload_property_data<float,OVM::Entity::Cell>(mesh_, *c_prop, prop_filters_,  vol_rend_);
                    } else if ((*c_prop)->typeNameWrapper()=="bool") {
                        upload_property_data<bool,OVM::Entity::Cell>(mesh_, *c_prop, prop_filters_,  vol_rend_);
                    } else if ((*c_prop)->typeNameWrapper()=="vec3d") {
                        upload_property_data<OVM::Vec3d,OVM::Entity::Cell>(mesh_, *c_prop, prop_filters_,  vol_rend_);
                    }
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Cell Props
        }

        if (mesh_.n_halfedge_props()>0 && ImGui::BeginMenu("Halfedges"))
        {
            for (auto he_prop = mesh_.halfedge_props_begin(); he_prop != mesh_.halfedge_props_end(); ++he_prop) {
                ImGui::PushID((*he_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*he_prop)->name().c_str(), (*he_prop)->typeNameWrapper().c_str()).c_str())) {
                    //
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Halfedge Props
        }

        if (mesh_.n_halfface_props()>0 && ImGui::BeginMenu("Halffaces"))
        {
            for (auto hf_prop = mesh_.halfface_props_begin(); hf_prop != mesh_.halfface_props_end(); ++hf_prop) {
                ImGui::PushID((*hf_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*hf_prop)->name().c_str(), (*hf_prop)->typeNameWrapper().c_str()).c_str())) {
                    //
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Halfface Props
        }

        ImGui::EndMenu(); // Properties
    }

    if (prop_)
    {
        //(*prop_)->typeNameWrapper().c_str()
        std::string prop_title = (*prop_)->name();
        ImGui::SeparatorText(prop_title.c_str());

        if (!prop_filters_.empty()) {

            if (ImGui::BeginMenu("Change Filter")) {

                for (int i = 0; i < prop_filters_.size(); ++i) {
                    if (ImGui::MenuItem(prop_filters_[i]->name().c_str())) {
                        active_prop_filter_ = i;
                    }
                }
                ImGui::EndMenu();
            }

            prop_filters_[active_prop_filter_]->renderUI(vol_rend_);
        }

        if (ImGui::Button("Unselect Property")) {
            prop_ = std::nullopt;
            vol_rend_.edge_property_.vis_ = VolumeMeshRenderer::Property::Visualization::COLOR;
            vol_rend_.vertex_property_.vis_ = VolumeMeshRenderer::Property::Visualization::COLOR;
            vol_rend_.face_property_.vis_ = VolumeMeshRenderer::Property::Visualization::COLOR;
            vol_rend_.cell_property_.vis_ = VolumeMeshRenderer::Property::Visualization::COLOR;
            init(scene);
        }
    }

    //---------------------------

}

}
