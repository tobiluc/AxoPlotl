#include "ConvexPolygonNode.h"

namespace AxoPlotl
{

void ConvexPolygonNode::init(Scene* scene)
{
    VolumeMeshRenderer::StaticData data;
    data.n_faces_ = 1;
    for (uint i = 0; i < vertices_.size(); ++i) {
        const auto& p = vertices_[i];
        data.positions_.push_back(Vec4f(p[0],p[1],p[2],1));
    }
    for (uint i = 1; i < vertices_.size()-1; ++i) {
        data.face_triangle_draw_vertices_.push_back({
            .vertex_index=0, .face_index = 0
        });
        data.face_triangle_draw_vertices_.push_back({
            .vertex_index=i, .face_index = 0
        });
        data.face_triangle_draw_vertices_.push_back({
            .vertex_index=i+1, .face_index = 0
        });
    }
    vol_rend_.init(data);

    // Compute Bounding Box
    bbox_.first = Vec3f(std::numeric_limits<float>::infinity());
    bbox_.second = Vec3f(-std::numeric_limits<float>::infinity());
    for (uint32_t i = 0; i < vertices_.size(); ++i) {
        for (int a = 0; a < 3; ++a) {
            bbox_.first[a] = std::min(bbox_.first[a], vertices_[i][a]);
            bbox_.second[a] = std::max(bbox_.second[a], vertices_[i][a]);
        }
    }
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

    // Confirm
    if (ImGui::Button("Confirm")) {

        // Update Renderer
        this->init(scene);
    }
}

}
