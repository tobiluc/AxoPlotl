#include "ConvexPolygonNode.h"
#include "AxoPlotl/utils/Utils.h"

namespace AxoPlotl
{

void ConvexPolygonNode::addToRenderer(Scene* scene)
{
    //bbox_.compute({vertices_[0],vertices_[1],vertices_[2]});

    PolyhedralMesh mesh;
    std::vector<OVM::VH> vhs;
    for (const auto& v : vertices_) {vhs.push_back(mesh.add_vertex(toVec3<OVM::Vec3d>(v)));}
    mesh.add_face(vhs);

    mesh_renderer_.init(mesh);
}

void ConvexPolygonNode::renderUIBody(Scene* scene)
{
    //-------------------
    // Text
    //-------------------
    for (int i = 0; i < vertices_.size(); ++i) {
        ImGui::PushID(i);
        char lbl[3];
        ImGui::Text("%d", i);
        ImGui::SameLine();
        if (ImGui::InputFloat3("##xx", &vertices_[i].x)) {}
        ImGui::PopID();
    }

    // Add/Remove Vertices
    ImGui::SameLine();
    if (vertices_.size()>3 && ImGui::Button("-")) {vertices_.erase(vertices_.end()-1);}
    ImGui::SameLine();
    if (ImGui::Button("+")) {vertices_.push_back(Vec3f(0,0,0));}

    // Set Fill
    ImGui::Checkbox("Fill", &fill_);

    // Confirm
    if (ImGui::Button("Confirm")) {

        // Update Renderer
        this->addToRenderer(scene);
    }
}

}
