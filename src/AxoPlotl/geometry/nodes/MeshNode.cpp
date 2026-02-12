#include "MeshNode.h"
#include "AxoPlotl/properties/property_data.hpp"
#include "AxoPlotl/utils/string_format.hpp"

namespace AxoPlotl
{

static GL::MeshRenderer::Data create_render_data(const VolumeMesh &mesh)
{
    GL::MeshRenderer::Data data;

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {

        // Compute smooth normal
        Vec3f n(0,0,0);
        for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it) {
            n += toVec3<Vec3f>(mesh.normal(vf_it->halfface_handle(0)));
        }
        n = glm::normalize(n);
        auto p =toVec3<Vec3f>(mesh.vertex(*v_it));

        data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{
            .position = p,
            .color = color_on_sphere(p)
        });
    }

    for (auto v_it = mesh.v_iter(); v_it.is_valid(); ++v_it) {
        data.pointIndices.push_back(v_it->uidx());
    }

    for (auto e_it = mesh.e_iter(); e_it.is_valid(); ++e_it) {
        auto vh0 = mesh.from_vertex_handle(e_it->halfedge_handle(0));
        auto vh1 = mesh.from_vertex_handle(e_it->halfedge_handle(1));

        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(vh0)),
            .color = Vec4f(0,0,0,1)
        });
        data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
            .position = toVec3<Vec3f>(mesh.vertex(vh1)),
            .color = Vec4f(0,0,0,1)
        });
        data.lineIndices.push_back(data.lineAttribs.size()-2);
        data.lineIndices.push_back(data.lineAttribs.size()-1);

    }

    int idx(0);
    for (auto f_it = mesh.f_iter(); f_it.is_valid(); ++f_it) {
        const auto& vhs = mesh.get_halfface_vertices(f_it->halfface_handle(0));
        auto normal = toVec3<Vec3f>(mesh.normal(f_it->halfface_handle(0)));

        for (OVM::VH vh : vhs) {
            auto p = toVec3<Vec3f>(mesh.vertex(vh));
            data.triangleAttribs.push_back(GL::MeshRenderer::VertexTriangleAttrib{
                .position = p,
                .color = color_on_sphere(p),
                .normal = normal,
                .buffer = 0.0f
            });
        }

        for (uint j = 1; j < vhs.size()-1; ++j) {
            data.triangleIndices.push_back(idx);
            data.triangleIndices.push_back(idx+j);
            data.triangleIndices.push_back(idx+j+1);
        }
        idx += vhs.size();
    }

    return data;
}

void MeshNode::init(Scene* scene)
{
    //renderer_.addMesh(mesh_, renderLoc_);
    GL::MeshRenderer::Data data = create_render_data(mesh_);
    //GL::MeshRenderer::createData(mesh_, data);
    mesh_renderer_.updateData(data);

    // Compute Bounding Box
    bbox_ = {Vec3f(std::numeric_limits<float>::infinity()), Vec3f(-std::numeric_limits<float>::infinity())};
    for (uint32_t i = 0; i < data.pointAttribs.size(); ++i) {
        for (int a = 0; a < 3; ++a) {
            bbox_.first[a] = std::min(bbox_.first[a], data.pointAttribs[i].position[a]);
            bbox_.second[a] = std::max(bbox_.second[a], data.pointAttribs[i].position[a]);
        }
    }
}

void MeshNode::renderUIBody(Scene* scene)
{
    // if (ImGui::Button("Unvisualize Properties")) {
    //     initRenderer(scene);
    // }


    if (ImGui::BeginMenu("Properties"))
    {
        if (mesh_.n_vertex_props()>0 && ImGui::BeginMenu("Vertices"))
        {
            for (auto v_prop = mesh_.vertex_props_begin(); v_prop != mesh_.vertex_props_end(); ++v_prop) {
                ImGui::PushID((*v_prop)->name().c_str());
                if (ImGui::MenuItem(string_format("%s [%s]", (*v_prop)->name().c_str(), (*v_prop)->typeNameWrapper().c_str()).c_str())) {
                    prop_ = *v_prop;
                    upload_property_data(mesh_, *v_prop, prop_filter, mesh_renderer_);
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
                    upload_property_data(mesh_, *e_prop, prop_filter, mesh_renderer_);
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
                    upload_property_data(mesh_, *f_prop, prop_filter, mesh_renderer_);
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
                    //
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

        ImGui::Text("%s [%s]", (*prop_)->name().c_str(), (*prop_)->typeNameWrapper().c_str());

        if (ImGui::BeginMenu("Visualization Options")) {
            if (ImGui::MenuItem("Range")) {
            }
            if (ImGui::MenuItem("Exact")) {

            }
            ImGui::EndMenu();
        }

        if (prop_filter) {
            prop_filter->renderUI(mesh_renderer_);
        }

        if (ImGui::Button("Stop Property Visualization")) {
            prop_ = std::nullopt;
            mesh_renderer_.settings().useDataForLineColor = true;
            mesh_renderer_.settings().useDataForPointColor = true;
            mesh_renderer_.settings().useDataForTriangleColor = true;
            init(scene);
        }
    }


    //---------------------------

}

}
