#include "ConvexPolygonNode.h"

namespace AxoPlotl
{

void ConvexPolygonNode::addToRenderer(Scene* scene)
{
    //bbox_.compute({vertices_[0],vertices_[1],vertices_[2]});

    renderer_.addConvexPolygon(vertices_, Color::LIGHTGRAY, fill_, renderLoc_);
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
        renderer_.clear(renderLoc_);
        this->addToRenderer(scene);
    }
}

}
