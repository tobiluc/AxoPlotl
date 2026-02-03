#include "MeshNode.h"
#include "AxoPlotl/IO/FileUtils.h"
#include "AxoPlotl/rendering/property_visualization.h"
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl
{

void MeshNode::initRenderer(Scene* scene)
{
    //renderer_.addMesh(mesh_, renderLoc_);
    GL::MeshRenderer::Data data;
    GL::MeshRenderer::createData(mesh_, data);
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

void MeshNode::visualizeSelectedProperty()
{
    if (!selected_psb_) {return;}

    mesh_renderer_.settings().useGlobalPointColor = false;
    mesh_renderer_.settings().useGlobalLineColor = false;
    mesh_renderer_.settings().useGlobalTriangleColor = false;

    switch (selected_psb_->entity_type()) {
    case OpenVolumeMesh::EntityType::Vertex:
        if (selected_psb_->typeNameWrapper() == "double") {
            auto prop = mesh_.request_vertex_property<double>(selected_psb_->name());

            // Find range
            auto range = PropVis::computeRange<double,OVM::VH>(mesh_, prop);

            // Define Rule
            PropVis::Predicate<double> pred([](double val) -> bool {return true;});
            PropVis::Action<OVM::VH> act([&](OVM::VH vh, GL::MeshRenderer::Data& data) -> void {
                double val = prop[vh];
                double t = (val - range.first) / (range.second - range.first);
                data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{
                    .position = toVec3<Vec3f>(mesh_.vertex(vh)),
                    .color = Color(t,0,1-t)
                });
                data.pointIndices.push_back(data.pointAttribs.size()-1);
            });
            PropVis::Rule<double,OVM::VH> rule(pred, act);
            std::vector<PropVis::Rule<double,OVM::VH>> rules = {rule};

            // Create Render Data
            GL::MeshRenderer::Data data;
            PropVis::generateRenderData<double,OVM::VH>(mesh_, prop, rules, data);
            mesh_renderer_.updateData(data);

        } else if (selected_psb_->typeNameWrapper() == "int") {
            auto prop = mesh_.request_vertex_property<int>(selected_psb_->name());

            // Define Rule
            PropVis::Predicate<int> pred([](int val) -> bool {return true;});
            std::unordered_map<int,Color> colors;
            PropVis::Action<OVM::VH> act([&](OVM::VH vh, GL::MeshRenderer::Data& data) -> void {
                int val = prop[vh];
                if (!colors.contains(val)) {colors[val] = Color::random();}
                data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{
                    .position = toVec3<Vec3f>(mesh_.vertex(vh)),
                    .color = colors[val]
                });
                data.pointIndices.push_back(data.pointAttribs.size()-1);
            });
            PropVis::Rule<int,OVM::VH> rule(pred, act);
            std::vector<PropVis::Rule<int,OVM::VH>> rules = {rule};

            // Create Render Data
            GL::MeshRenderer::Data data;
            PropVis::generateRenderData<int,OVM::VH>(mesh_, prop, rules, data);
            mesh_renderer_.updateData(data);

        } else if (selected_psb_->typeNameWrapper() == "bool") {

            auto prop = mesh_.request_vertex_property<bool>(selected_psb_->name());

            // Define Rule
            PropVis::Predicate<bool> pred([](bool val) -> bool {return true;});
            PropVis::Action<OVM::VH> act([&](OVM::VH vh, GL::MeshRenderer::Data& data) -> void {
                bool val = prop[vh];
                Color color = val? Color::GREEN : Color::RED;
                data.pointAttribs.push_back(GL::MeshRenderer::VertexPointAttrib{
                    .position = toVec3<Vec3f>(mesh_.vertex(vh)),
                    .color = color
                });
                data.pointIndices.push_back(data.pointAttribs.size()-1);
            });
            PropVis::Rule<bool,OVM::VH> rule(pred, act);
            std::vector<PropVis::Rule<bool,OVM::VH>> rules = {rule};

            // Create Render Data
            GL::MeshRenderer::Data data;
            PropVis::generateRenderData<bool,OVM::VH>(mesh_, prop, rules, data);
            mesh_renderer_.updateData(data);

        } else if (selected_psb_->typeNameWrapper() == "vec3d") {
            auto prop = mesh_.request_vertex_property<OVM::Vec3d>(selected_psb_->name());

            // Define Rule
            PropVis::Predicate<OVM::Vec3d> pred([](const OVM::Vec3d& v) -> bool {return true;});
            PropVis::Action<OVM::VH> act([&](OVM::VH vh, GL::MeshRenderer::Data& data) -> void {
                const OVM::Vec3d& v = prop[vh];

                data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
                    .position = toVec3<Vec3f>(mesh_.vertex(vh)),
                    .color = getColorOnSphere(v[0],v[1],v[2])
                });

                data.lineAttribs.push_back(GL::MeshRenderer::VertexLineAttrib{
                    .position = toVec3<Vec3f>(mesh_.vertex(vh) + v),
                    .color = getColorOnSphere(v[0],v[1],v[2])
                });

                data.lineIndices.push_back(data.lineAttribs.size()-2);
                data.lineIndices.push_back(data.lineAttribs.size()-1);
            });
            PropVis::Rule<OVM::Vec3d,OVM::VH> rule(pred, act);
            std::vector<PropVis::Rule<OVM::Vec3d,OVM::VH>> rules = {rule};

            // Create Render Data
            GL::MeshRenderer::Data data;
            PropVis::generateRenderData<OVM::Vec3d,OVM::VH>(mesh_, prop, rules, data);
            mesh_renderer_.updateData(data);
        }
    case OpenVolumeMesh::EntityType::Edge:
    case OpenVolumeMesh::EntityType::HalfEdge:
    case OpenVolumeMesh::EntityType::Face:
    case OpenVolumeMesh::EntityType::HalfFace:
    case OpenVolumeMesh::EntityType::Cell:
    case OpenVolumeMesh::EntityType::Mesh:
        break;
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
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*v_prop)->name().c_str(), (*v_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_= *v_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_edge_props()>0 && ImGui::BeginMenu("Edges"))
        {
            for (auto e_prop = mesh_.edge_props_begin(); e_prop != mesh_.edge_props_end(); ++e_prop) {
                ImGui::PushID((*e_prop)->name().c_str());
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*e_prop)->name().c_str(), (*e_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_= *e_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_face_props()>0 && ImGui::BeginMenu("Faces"))
        {
            for (auto f_prop = mesh_.face_props_begin(); f_prop != mesh_.face_props_end(); ++f_prop) {
                ImGui::PushID((*f_prop)->name().c_str());
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*f_prop)->name().c_str(), (*f_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_= *f_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_cell_props()>0 && ImGui::BeginMenu("Cells"))
        {
            for (auto c_prop = mesh_.cell_props_begin(); c_prop != mesh_.cell_props_end(); ++c_prop) {
                ImGui::PushID((*c_prop)->name().c_str());
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*c_prop)->name().c_str(), (*c_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_= *c_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_halfedge_props()>0 && ImGui::BeginMenu("Halfedges"))
        {
            for (auto he_prop = mesh_.halfedge_props_begin(); he_prop != mesh_.halfedge_props_end(); ++he_prop) {
                ImGui::PushID((*he_prop)->name().c_str());
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*he_prop)->name().c_str(), (*he_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_= *he_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        if (mesh_.n_halfface_props()>0 && ImGui::BeginMenu("Halffaces"))
        {
            for (auto hf_prop = mesh_.halfface_props_begin(); hf_prop != mesh_.halfface_props_end(); ++hf_prop) {
                ImGui::PushID((*hf_prop)->name().c_str());
                if (ImGui::MenuItem(IO::string_format("%s [%s]", (*hf_prop)->name().c_str(), (*hf_prop)->typeNameWrapper().c_str()).c_str())) {
                    selected_psb_ = *hf_prop;
                }
                ImGui::PopID();
            }

            ImGui::EndMenu(); // Vertex Props
        }

        ImGui::EndMenu(); // Properties
    }

    if (selected_psb_) {
        ImGui::Text("%s", selected_psb_->name().c_str());

        // --- Rules

        if (ImGui::Button("Visualize")) {
            visualizeSelectedProperty();
        }
    }

    //---------------------------

}

}
